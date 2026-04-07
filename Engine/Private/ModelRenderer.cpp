#include "pch.h"
#include "ModelRenderer.h"
#include "Camera.h"
#include "Transform.h"

ModelRenderer::ModelRenderer(shared_ptr<Shader> shader)
	: Component(ComponentType::ModelRenderer)
	, _shader(shader)
{
}

ModelRenderer::~ModelRenderer()
{
}




HRESULT ModelRenderer::Render()
{
	if (_model == nullptr)
		return E_FAIL;

	//GlobalData
	_shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
	auto world = GetTransform()->GetWorldMatrix();
	_shader->PushTransformData(TransformDesc{ world });

	// TODO : Light
	//auto lightObj = SCENE->GetCurrentScene()->GetLight();
	//if (lightObj)
	//	_shader->PushLightData(lightObj->GetLight()->GetLightDesc());

	//Bones
	BoneDesc boneDesc;

	const uint32 boneCount = _model->GetBoneCount();
	for (uint32 i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
		boneDesc.transforms[i] = bone->transform;
	}
	_shader->PushBoneData(boneDesc);

	const auto& meshes = _model->GetMeshes();
	for (auto& mesh : meshes)
	{
		if (mesh->material)
			mesh->material->Update();

		//BoneIndex
		_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		//IA
		mesh->vertexBuffer->PushData();
		mesh->indexBuffer->PushData();

		_shader->DrawIndexed(_techniqueIndex, _pass, mesh->indexBuffer->GetCount(), 0, 0);
	}

	return S_OK;
}

void ModelRenderer::SetModel(shared_ptr<Model> model)
{
	_model = model;

	const auto& materials = _model->GetMaterials();
	for (auto& material : materials)
	{
		material->SetShader(_shader);
	}
}

void ModelRenderer::OnInspectorGUI()
{
	if (ImGui::CollapsingHeader("ModelRenderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (_shader)
		{
			string shaderName = _shader ? Utils::ToString(_shader->GetName()) : "None";
			ImGui::Text("Shader:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), shaderName.c_str());

			// Shader가 가진 Technique 개수만큼 순회하며 이름을 가져옴.
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
		}

		ImGui::Separator();

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
	}
	ImGui::Separator();
}