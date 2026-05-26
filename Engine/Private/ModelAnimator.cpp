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

	_shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
	// Transform
	auto world = GetTransform()->GetWorldMatrix();
	_shader->PushTransformData(TransformDesc{ world });

	_shader->PushTweenTempData(_tweenDesc);
	//_shader->PushKeyframeData(_keyframeDesc);

	// SRV를 통해 정보 전달
	_shader->GetSRV("TransformMap")->SetResource(_model->GetTransformSRV().Get());

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


	_shadowShader->GetSRV("TransformMap")->SetResource(_model->GetTransformSRV().Get());

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

float ModelAnimator::GetProgress()
{
	// 현재 재생 중인 애니메이션 정보 가져오기
	shared_ptr<ModelAnimation> currentAnim = _model->GetAnimationByIndex(_tweenDesc.curr.animIndex);
	if (currentAnim == nullptr || currentAnim->frameCount <= 1)
		return 0.0f;

	// 현재 프레임과 ratio를 합쳐 실제 위치 계산
	float currentPos = static_cast<float>(_tweenDesc.curr.currFrame) + _tweenDesc.curr.ratio;

	// 전체 프레임 수로 나누어 진행률 반환
	return currentPos / static_cast<float>(currentAnim->frameCount-1);
}

bool ModelAnimator::IsCurrentAnimFinished()
{
	if (_model == nullptr) return false;

	// 현재 다음 애니메이션으로 넘어가는 중(Tweening)이라면 아직 끝난 게 아님
	if (_tweenDesc.next.animIndex >= 0)
	{
		// 만약 '보간이 거의 끝나감'을 알고 싶다면 tweenRatio를 체크
		return false;
	}

	shared_ptr<ModelAnimation> currentAnim = _model->GetAnimationByIndex(_tweenDesc.curr.animIndex);
	if (currentAnim == nullptr) return false;

	// 단일 애니메이션 재생 중 마지막 프레임 도달 여부 체크
	// 현재 프레임이 마지막일 때
	if (_tweenDesc.curr.currFrame >= currentAnim->frameCount - 1)
	{
		return true;
	}

	return false;
}
wstring ModelAnimator::GetCurAnimStateName()
{
	 return _animFSM->GetCurAnimState()->GetName();
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
			string curStateName = _animFSM ? Utils::ToString(_animFSM->GetCurAnimState()->GetName()) : "None";
			ImGui::Text("animFSM: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), fsmName.c_str());
			ImGui::Text("animFSM: ");
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), curStateName.c_str());

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