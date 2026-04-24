#include "pch.h"
#include "ModelRenderer.h"
#include "Camera.h"
#include "Transform.h"
#include "Light.h"
ModelRenderer::ModelRenderer()
	: Component(ComponentType::ModelRenderer)
{
}

ModelRenderer::ModelRenderer(shared_ptr<Shader> shader)
	: Component(ComponentType::ModelRenderer)
	, _shader(shader)
{
}

ModelRenderer::~ModelRenderer()
{
}




void ModelRenderer::Update()
{
	//TODO 임시, 렌더그룹변경하자
	GAME.Add_RenderObject(RENDERGROUP::NONBLEND, GetGameObject());
}

HRESULT ModelRenderer::Render()
{
	if (_model == nullptr)
		return S_OK;
	if (_shader == nullptr)
		return S_OK;

	//GlobalData
	_shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
	auto world = GetTransform()->GetWorldMatrix();
	_shader->PushTransformData(TransformDesc{ world });

	// TODO : Light
	//auto lightObj = SCENE->GetCurrentScene()->GetLight();
	//if (lightObj)
	//	_shader->PushLightData(lightObj->GetLight()->GetLightDesc());

	//LightDesc lightDesc;
	//lightDesc.ambient = Vec4(0.55f, 0.55f, 0.6f, 1.0f);
	//lightDesc.diffuse = Vec4(1.f);
	//lightDesc.specular = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
	//lightDesc.direction = Vec3(-0.5f, -0.8f, 1.0f);
	//lightDesc.direction.Normalize();
	//_shader->PushLightData(lightDesc);

	auto& lightList = GAME.GetLigthList();
	if (!lightList.empty())
	{
		auto& lightDesc = lightList.front()->GetLightDesc();
		_shader->PushLightData(lightDesc);
	}

	//Bones
	//BoneDesc boneDesc;
	//
	//const uint32 boneCount = _model->GetBoneCount();
	//for (uint32 i = 0; i < boneCount; ++i)
	//{
	//	shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
	//	boneDesc.transforms[i] = bone->transform;
	//}
	//_shader->PushBoneData(boneDesc);

	// 모델의 월드 행렬의 역행렬 계산
	Matrix invWorldMat = world.Invert();

	// 2. 카메라 Frustum을 모델의 로컬 공간으로 변환
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

		//BoneIndex
		_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		//IA
		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
	GuiRemoveButton("ModelRenderer");
	if (ImGui::CollapsingHeader("ModelRenderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
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
					if(_model)
						SetModel(_model);
				}
			}
			ImGui::EndPopup();
		}

		if (_shader)
		{
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

			ImGui::Separator();

			// --- Model 정보 ---
			string modelName = _model ? Utils::ToString(_model->GetName()) : "None";
			ImGui::Text("Model: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), modelName.c_str());

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
	}
	ImGui::Separator();
}