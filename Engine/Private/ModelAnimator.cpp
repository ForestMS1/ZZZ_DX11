#include "pch.h"
#include "ModelAnimator.h"
#include "Material.h"
#include "ModelMesh.h"
#include "Model.h"
#include "ModelAnimation.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"
#include "AnimFSM.h"
#include "AnimState.h"
#include "Transition.h"
#include "Light.h"
#include <filesystem>
ModelAnimator::ModelAnimator()
	: Component(ComponentType::Animator)
{
}
ModelAnimator::ModelAnimator(shared_ptr<Shader> shader)
	: Component(ComponentType::Animator), _shader(shader)
{

}

ModelAnimator::~ModelAnimator()
{

}

void ModelAnimator::Awake()
{
	_shadowShader = GAME.GetResource<Shader>(L"ShadowAnim.fx");
	if (_shadowShader == nullptr)
	{
		_shadowShader = Shader::Create(L"ShadowAnim.fx");
		GAME.AddResource<Shader>(L"ShadowAnim.fx", _shadowShader);
	}
}

void ModelAnimator::Update()
{
	if (_model == nullptr)
		return;

	if (_texture == nullptr)
		CreateTexture();

	//TODO 임시, 렌더그룹변경하자
	GAME.Add_RenderObject(_renderGroup, GetGameObject());

	TweenDesc& desc = _tweenDesc;

	if(_animFSM)
		_animFSM->Update();

	desc.curr.sumTime += DT;
	// 현재 애니메이션
	{
		shared_ptr<ModelAnimation> currentAnim = _model->GetAnimationByIndex(desc.curr.animIndex);
		if (currentAnim)
		{
			// 시간이 흐름에 따라서 프레임을 바꿔준다.
			float timePerFrame = 1 / (currentAnim->frameRate * desc.curr.speed);
			if (desc.curr.sumTime >= timePerFrame)
			{
				desc.curr.sumTime = 0;
				desc.curr.currFrame = (desc.curr.currFrame + 1) % currentAnim->frameCount;
				desc.curr.nextFrame = (desc.curr.currFrame + 1) % currentAnim->frameCount;
			}

			desc.curr.ratio = (desc.curr.sumTime / timePerFrame);
		}
	}

	// 다음 애니메이션이 예약 되어 있다면 
	if (desc.next.animIndex >= 0)
	{
		if (_animFSM)
			desc.tweenDuration = _animFSM->GetCurTransition()->GetTransitionDuration();

		desc.tweenSumTime += DT;
		desc.tweenRatio = desc.tweenSumTime / desc.tweenDuration;

		if (desc.tweenRatio >= 1.f)
		{
			// 애니메이션 교체 성공
			desc.curr = desc.next;
			desc.ClearNextAnim();
			if (_animFSM)
				_animFSM->ChangeState(_animFSM->GetCurTransition()->GetToState()->GetName());
		}
		else
		{
			// 프레임 교체중
			shared_ptr<ModelAnimation> nextAnim = _model->GetAnimationByIndex(desc.next.animIndex);
			desc.next.sumTime += DT;

			float timePerFrame = 1.f / (nextAnim->frameRate * desc.next.speed);

			if (desc.next.ratio >= 1.f)
			{
				desc.next.sumTime = 0;

				desc.next.currFrame = (desc.next.currFrame + 1) % nextAnim->frameCount;
				desc.next.nextFrame = (desc.next.currFrame + 1) % nextAnim->frameCount;
			}

			desc.next.ratio = desc.next.sumTime / timePerFrame;
		}
	}
}

HRESULT ModelAnimator::Render()
{
	if (_model == nullptr)
		return E_FAIL;
	if (_shader == nullptr)
		return E_FAIL;
	if (_texture == nullptr)
		CreateTexture();

	_shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
	// Transform
	auto world = GetTransform()->GetWorldMatrix();
	_shader->PushTransformData(TransformDesc{ world });

	_shader->PushTweenTempData(_tweenDesc);
	//_shader->PushKeyframeData(_keyframeDesc);

	// SRV를 통해 정보 전달
	_shader->GetSRV("TransformMap")->SetResource(_srv.Get());

	//auto& lightList = GAME.GetLigthList();
	//if (!lightList.empty())
	//{
	//	auto& lightDesc = lightList.front()->GetLightDesc();
	//	_shader->PushLightData(lightDesc);
	//
	//	const Matrix& lightView = lightList.front()->GetLighViewMatrix();
	//	const Matrix& lightProj = lightList.front()->GetLighProjMatrix();
	//	_shader->GetMatrix("g_LightView")->SetMatrix((float*)&lightView);
	//	_shader->GetMatrix("g_LightProj")->SetMatrix((float*)&lightProj);
	//}
	
	// Bones
	BoneDesc boneDesc;

	const uint32 boneCount = _model->GetBoneCount();
	for (uint32 i = 0; i < boneCount; i++)
	{
		shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
		boneDesc.transforms[i] = bone->transform;
	}
	_shader->PushBoneData(boneDesc);

	// 모델의 월드 행렬의 역행렬 계산
	Matrix invWorldMat = world.Invert();

	// 카메라 Frustum을 모델의 로컬 공간으로 변환
	BoundingFrustum localFrustum;
	Camera::S_Frustum.Transform(localFrustum, invWorldMat);
	
	const auto& meshes = _model->GetMeshes();
	for (auto& mesh : meshes)
	{
		// 충돌 검사 (시야 밖에 있다면 통과)
		if (localFrustum.Contains(mesh->boundingBox) == DirectX::DISJOINT)
			continue;

		if (mesh->material)
			mesh->material->Update();

		// BoneIndex
		_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mesh->vertexBuffer->PushData();
		mesh->indexBuffer->PushData();

		_shader->DrawIndexed(_techniqueIndex, _pass, mesh->indexBuffer->GetCount(), 0, 0);
		//GAME.DrawBox(mesh->boundingBox, Colors::Lime, Camera::S_MatView, Camera::S_MatProjection, world);
	}
	return S_OK;
}

HRESULT ModelAnimator::RenderShadow()
{
	if (_model == nullptr)
		return E_FAIL;
	if (_shadowShader == nullptr)
		return E_FAIL;
	if (_texture == nullptr)
		CreateTexture();

	auto& lightList = GAME.GetLigthList();
	if (!lightList.empty())
	{
		auto& lightDesc = lightList.front()->GetLightDesc();
		_shadowShader->PushLightData(lightDesc);

		const Matrix& lightView = lightList.front()->GetLighViewMatrix();
		const Matrix& lightProj = lightList.front()->GetLighProjMatrix();
		_shadowShader->GetMatrix("g_LightView")->SetMatrix((float*)&lightView);
		_shadowShader->GetMatrix("g_LightProj")->SetMatrix((float*)&lightProj);
	}
	// Transform
	auto gameObjectWorld = GetTransform()->GetWorldMatrix();
	_shadowShader->PushTransformData(TransformDesc{ gameObjectWorld });

	_shadowShader->PushTweenTempData(_tweenDesc);


	_shadowShader->GetSRV("TransformMap")->SetResource(_srv.Get());

	BoneDesc boneDesc;

	const uint32 boneCount = _model->GetBoneCount();
	for (uint32 i = 0; i < boneCount; i++)
	{
		shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
		boneDesc.transforms[i] = bone->transform;
	}
	_shadowShader->PushBoneData(boneDesc);


	const auto& meshes = _model->GetMeshes();
	for (auto& mesh : meshes)
	{
		// BoneIndex
		_shadowShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		//IA
		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mesh->vertexBuffer->PushData();
		mesh->indexBuffer->PushData();

		_shadowShader->DrawIndexed(0, 0, mesh->indexBuffer->GetCount(), 0, 0);
	}

	return S_OK;
}

void ModelAnimator::SetModel(shared_ptr<Model> model)
{
	_model = model;

	const auto& materials = _model->GetMaterials();
	for (auto& material : materials)
	{
		material->SetShader(_shader);
	}
}

void ModelAnimator::CreateTexture()
{
	if (_model->GetAnimationCount() == 0)
		return;

	_animTransforms.resize(_model->GetAnimationCount());
	for (uint32 i = 0; i < _model->GetAnimationCount(); i++)
		CreateAnimationTransform(i);

	// Creature Texture
	{
		//		     Bone1  Bone2  Bone3 ...  Bone350
		//	Frame1   [SRT]  [SRT]  [SRT]  ...  [SRT]
		//  Frame2   [SRT]
		//  Frame3   [SRT]
		//	...		 ...
		//	Frame500 [SRT]
		//
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = _model->GetAnimationCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16바이트
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		const uint32 dataSize = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
		const uint32 pageSize = dataSize * MAX_MODEL_KEYFRAMES;
		void* mallocPtr = ::malloc(pageSize * _model->GetAnimationCount());

		// 파편화된 데이터를 조립한다.
		for (uint32 c = 0; c < _model->GetAnimationCount(); c++)
		{
			uint32 startOffset = c * pageSize;

			BYTE* pageStartPtr = reinterpret_cast<BYTE*>(mallocPtr) + startOffset;

			for (uint32 f = 0; f < MAX_MODEL_KEYFRAMES; f++)
			{
				void* ptr = pageStartPtr + dataSize * f;
				::memcpy(ptr, _animTransforms[c].transforms[f].data(), dataSize);
			}
		}

		// 리소스 만들기
		vector<D3D11_SUBRESOURCE_DATA> subResources(_model->GetAnimationCount());

		for (uint32 c = 0; c < _model->GetAnimationCount(); c++)
		{
			void* ptr = (BYTE*)mallocPtr + c * pageSize;
			subResources[c].pSysMem = ptr;
			subResources[c].SysMemPitch = dataSize;
			subResources[c].SysMemSlicePitch = pageSize;
		}

		HRESULT hr = DEVICE->CreateTexture2D(&desc, subResources.data(), _texture.GetAddressOf());
		CHECK(hr);

		::free(mallocPtr);
	}

	// Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = _model->GetAnimationCount();

		HRESULT hr = DEVICE->CreateShaderResourceView(_texture.Get(), &desc, _srv.GetAddressOf());
		CHECK(hr);
	}
}

void ModelAnimator::CreateAnimationTransform(uint32 index)
{
	vector<Matrix> tempAnimBoneTransforms(MAX_MODEL_TRANSFORMS, Matrix::Identity);

	shared_ptr<ModelAnimation> animation = _model->GetAnimationByIndex(index);

	for (uint32 f = 0; f < animation->frameCount; f++)
	{
		for (uint32 b = 0; b < _model->GetBoneCount(); b++)
		{
			shared_ptr<ModelBone> bone = _model->GetBoneByIndex(b);

			Matrix matAnimation;

			shared_ptr<ModelKeyframe> frame = animation->GetKeyframe(bone->name);
			if (frame != nullptr)
			{
				ModelKeyframeData& data = frame->transforms[f];

				Matrix S, R, T;
				S = Matrix::CreateScale(data.scale.x, data.scale.y, data.scale.z);
				R = Matrix::CreateFromQuaternion(data.rotation);
				T = Matrix::CreateTranslation(data.translation.x, data.translation.y, data.translation.z);

				matAnimation = S * R * T;
			}
			else
			{
				matAnimation = Matrix::Identity;
			}

			// [ !!!!!!! ]
			Matrix toRootMatrix = bone->transform;
			Matrix invGlobal = toRootMatrix.Invert();

			int32 parentIndex = bone->parentIndex;

			Matrix matParent = Matrix::Identity;
			if (parentIndex >= 0)
				matParent = tempAnimBoneTransforms[parentIndex];

			tempAnimBoneTransforms[b] = matAnimation * matParent;

			// 결론
			_animTransforms[index].transforms[f][b] = invGlobal * tempAnimBoneTransforms[b];
		}
	}
}

void ModelAnimator::OnInspectorGUI()
{
	GuiRemoveButton("ModelAnimator");
	if (ImGui::CollapsingHeader("ModelAnimator", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		// RENDERGROUP 순회하면서 이름 가져옴
		uint8 renderGroupCount = static_cast<uint8>(RENDERGROUP::END);
		if (ImGui::BeginCombo("RenderGruop", Utils::ToString(RENDERGROUP_NAMES[static_cast<uint8>(_renderGroup)]).c_str()))
		{
			for (uint8 i = 0; i < renderGroupCount; ++i)
			{
				bool isSelected = (static_cast<uint8>(_renderGroup) == i);
				if (ImGui::Selectable(Utils::ToString(RENDERGROUP_NAMES[i]).c_str(), isSelected))
				{
					_renderGroup = static_cast<RENDERGROUP>(i);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();


		// Shader 변경 버튼
		if (ImGui::Button("Change Shader##Shader", ImVec2(-1, 0))) // 너비를 가득 채우려면 ImVec2(-1, 0)
		{
			ImGui::OpenPopup("ShaderResourceSearchPopup");
		}

		// 팝업 내부 로직
		if (ImGui::BeginPopup("ShaderResourceSearchPopup"))
		{
			ImGui::TextDisabled("Shaders");
			ImGui::Separator();

			const auto& resources = GAME.GetResourceArray();

			for (const auto& pair : resources[static_cast<uint8>(ResourceType::SHADER)])
			{
				const auto& shader = pair.second;
				if (ImGui::Selectable(Utils::ToString(shader->GetName()).c_str()))
				{
					_shader = static_pointer_cast<Shader>(shader);
					if (_model)
						SetModel(_model);
				}
			}
			ImGui::EndPopup();
		}

		// --- Shader 정보 ---
		string shaderName = _shader ? Utils::ToString(_shader->GetName()) : "None";
		ImGui::Text("Shader: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), shaderName.c_str());

		if (_shader == nullptr)
			return;

		// Shader가 가진 Technique 개수만큼 순회하며 이름을 가져옴
		uint32 techCount = _shader->GetTechniqueCount();
		string currentTechName = _shader->GetTechniqueName(_techniqueIndex);

		if (ImGui::BeginCombo("Technique", currentTechName.c_str()))
		{
			for (uint32 i = 0; i < techCount; ++i)
			{
				bool isSelected = (_techniqueIndex == i);
				if (ImGui::Selectable(_shader->GetTechniqueName(i).c_str(), isSelected))
				{
					_techniqueIndex = i;
					_pass = 0; // Technique이 바뀌면 Pass는 0으로 리셋해야 안전함
				}
			}
			ImGui::EndCombo();
		}

		// --- Pass 선택 ---
		uint32 maxPassCount = _shader->GetPassCount(_techniqueIndex);
		int p = static_cast<int>(_pass);

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
		// 해당 Technique이 가진 Pass 개수 내에서만 조절 가능하게 제한
		if (ImGui::SliderInt("Pass", &p, 0, maxPassCount - 1))
		{
			_pass = static_cast<uint32>(p);
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button("R##Pass")) _pass = 0;



		// Model 변경 버튼
		if (ImGui::Button("Change Model##Model", ImVec2(-1, 0))) // 너비를 가득 채우려면 ImVec2(-1, 0)
		{
			ImGui::OpenPopup("ModelResourceSearchPopup");
		}

		// 팝업 내부 로직
		if (ImGui::BeginPopup("ModelResourceSearchPopup"))
		{
			ImGui::TextDisabled("Models");
			ImGui::Separator();

			const auto& resources = GAME.GetResourceArray();

			for (const auto& pair : resources[static_cast<uint8>(ResourceType::MODEL)])
			{
				const auto& model = pair.second;
				if (ImGui::Selectable(Utils::ToString(model->GetName()).c_str()))
				{
					SetModel(static_pointer_cast<Model>(model));
				}
			}
			ImGui::EndPopup();
		}

		// --- Model 정보 ---
		string modelName = _model ? Utils::ToString(_model->GetName()) : "None";
		ImGui::Text("Model: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), modelName.c_str());

		// 모델 상세 정보
		if (_model && ImGui::TreeNodeEx("Model Details", ImGuiTreeNodeFlags_FramePadding))
		{
			ImGui::BulletText("Mesh Count: %d", _model->GetMeshCount());
			ImGui::BulletText("Animation Count: %d", _model->GetAnimationCount());


			ImGui::BulletText("Bone Count: %d", _model->GetBoneCount());

			ImGui::TreePop();
		}

		ImGui::Separator();

		// FSM
		if (_model)
		{
			// --- FSM 정보 ---
			string fsmName = _animFSM ? Utils::ToString(_animFSM->GetFileName()) : "None";
			ImGui::Text("animFSM: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), fsmName.c_str());

			// AnimFSM 변경 버튼
			if (ImGui::Button("Change AnimFSM##AnimFSM", ImVec2(-1, 0))) // 너비를 가득 채우려면 ImVec2(-1, 0)
			{
				ImGui::OpenPopup("AnimFSMSearchPopup");
			}

			// 팝업 내부 로직
			if (ImGui::BeginPopup("AnimFSMSearchPopup"))
			{
				ImGui::TextDisabled("AnimFSM");
				ImGui::Separator();

				std::string path = "../../Saved/FSM/"; // 파일명을 가져올 폴더 경로

				for (const auto& entry : filesystem::directory_iterator(path)) 
				{
					// 파일명만 출력
					const auto& fileName = entry.path().stem().string();
					if (ImGui::Selectable(fileName.c_str()))
					{
						shared_ptr<AnimFSM> fsm = make_shared<AnimFSM>(SHARED_THIS(ModelAnimator));
						fsm->Load(fileName, SHARED_THIS(ModelAnimator));
						SetFSM(fsm);
					}
				}
				ImGui::EndPopup();
			}
		}

		ImGui::Separator();
		

		ImGui::Unindent();
	}
	ImGui::Separator();

	static bool onEdit = false;
	if (ImGui::Button(onEdit ? "exitEdit" : "onEdit"))
		onEdit = !onEdit;
	if (_animFSM && onEdit)
		_animFSM->OnInspectorGUI();
}