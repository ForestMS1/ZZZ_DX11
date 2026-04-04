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

ModelAnimator::ModelAnimator(shared_ptr<Shader> shader)
	: Component(ComponentType::Animator), _shader(shader)
{

}

ModelAnimator::~ModelAnimator()
{

}

void ModelAnimator::Update()
{
	if (_model == nullptr)
		return;
	if (_texture == nullptr)
		CreateTexture();

	TweenDesc& desc = _tweenDesc;

	desc.curr.sumTime += DT;
	// 현재 애니메이션
	{
		shared_ptr<ModelAnimation> currentAnim = _model->GetAnimationByIndex(desc.curr.animIndex);
		if (currentAnim)
		{
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
		desc.tweenSumTime += DT;
		desc.tweenRatio = desc.tweenSumTime / desc.tweenDuration;

		if (desc.tweenRatio >= 1.f)
		{
			// 애니메이션 교체 성공
			desc.curr = desc.next;
			desc.ClearNextAnim();
		}
		else
		{
			// 교체중
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
	// Anim Update
	//ImGui::InputInt("AnimIndex", &desc.curr.animIndex);
	//_keyframeDesc.animIndex %= _model->GetAnimationCount();

	//static int32 nextAnimIndex = 0;
	//if (ImGui::InputInt("NextAnimIndex", &nextAnimIndex))
	//{
	//	nextAnimIndex %= _model->GetAnimationCount();
	//	desc.ClearNextAnim(); // 기존꺼 밀어주기
	//	desc.next.animIndex = nextAnimIndex;
	//}

	//if (_model->GetAnimationCount() > 0)
	//	desc.curr.animIndex %= _model->GetAnimationCount();

	//ImGui::InputFloat("Speed", &desc.curr.speed, 0.5f, 4.f);


}

HRESULT ModelAnimator::Render()
{
	if (_model == nullptr)
		return E_FAIL;
	if (_texture == nullptr)
		CreateTexture();

	TweenDesc& desc = _tweenDesc;

	_shader->PushTweenTempData(desc);

	// SRV를 통해 정보 전달
	_shader->GetSRV("TransformMap")->SetResource(_srv.Get());

	// Bones
	BoneDesc boneDesc;

	const uint32 boneCount = _model->GetBoneCount();
	for (uint32 i = 0; i < boneCount; i++)
	{
		shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
		boneDesc.transforms[i] = bone->transform;
	}
	_shader->PushBoneData(boneDesc);

	// Transform
	auto world = GetTransform()->GetWorldMatrix();
	_shader->PushTransformData(TransformDesc{ world });
	
	const auto& meshes = _model->GetMeshes();
	for (auto& mesh : meshes)
	{
		if (mesh->material)
			mesh->material->Update();

		// BoneIndex
		_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		mesh->vertexBuffer->PushData();
		mesh->indexBuffer->PushData();

		_shader->DrawIndexed(_techniqueIndex, _pass, mesh->indexBuffer->GetCount(), 0, 0);
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
	if (ImGui::CollapsingHeader("ModelAnimator", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		// --- Shader & Model 정보 (Renderer와 유사) ---
		string shaderName = _shader ? Utils::ToString(_shader->GetName()) : "None";
		string modelName = _model ? Utils::ToString(_model->GetName()) : "None";
		ImGui::Text("Shader: %s", shaderName.c_str());
		ImGui::Text("Model:  %s", modelName.c_str());

		ImGui::Separator();

		// --- Animation 제어 (KeyframeDesc / TweenDesc) ---
		if (ImGui::TreeNodeEx("Animation State", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 현재 애니메이션 인덱스 제어
			int animIdx = _keyframeDesc.animIndex;
			int maxAnimCount = _model ? (int)_model->GetAnimationCount() : 0;

			if (ImGui::SliderInt("Anim Index", &animIdx, 0, maxAnimCount - 1))
			{
				_keyframeDesc.animIndex = animIdx;
				// 필요 시 애니메이션 변경에 따른 초기화 로직 호출
			}

			// 프레임 정보 및 진행률 (보통 0.0 ~ 1.0)
			ImGui::SliderInt("Current Frame", (int*)&_keyframeDesc.currFrame, 0, 500); // MAX_MODEL_KEYFRAMES 참고
			ImGui::SliderFloat("Progress", &_keyframeDesc.ratio, 0.0f, 1.0f);

			if (ImGui::TreeNode("Tweening (Blending)"))
			{
				ImGui::SliderInt("Next Anim", (int*)&_tweenDesc.next.animIndex, 0, maxAnimCount - 1);
				ImGui::SliderFloat("Tween Ratio", &_tweenDesc.tweenRatio, 0.0f, 1.0f);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		ImGui::Separator();
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

		// --- [4] Debug: Texture/SRV 정보 ---
		if (ImGui::TreeNode("Internal Resources (Debug)"))
		{
			ImGui::Text("Anim Transforms: %d", (int)_animTransforms.size());
			if (_srv) ImGui::BulletText("Animation Texture (SRV) Created");
			ImGui::TreePop();
		}

		ImGui::Unindent();
	}
}