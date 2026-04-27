#include "pch.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Light.h"

MeshRenderer::MeshRenderer() : Component(ComponentType::MeshRenderer)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Awake()
{
	if (_shadowShader == nullptr)
	{
		_shadowShader = Shader::Create(L"ShadowNoAnim.fx");
	}
}

void MeshRenderer::Update()
{
	//TODO 임시
	GAME.Add_RenderObject(_renderGroup, GetGameObject());
}

HRESULT MeshRenderer::Render()
{
	if (_mesh == nullptr || _material == nullptr)
		return E_FAIL;

	auto shader = _material->GetShader();
	if (shader == nullptr)
		return E_FAIL;

	_material->Update();

	// GlobalData
	shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
	auto world = GetTransform()->GetWorldMatrix();
	shader->PushTransformData(TransformDesc{ world });

	//auto& lightList = GAME.GetLigthList();
	//if (!lightList.empty())
	//{
	//	auto& lightDesc = lightList.front()->GetLightDesc();
	//	shader->PushLightData(lightDesc);

	//	const Matrix& lightView = lightList.front()->GetLighViewMatrix();
	//	const Matrix& lightProj = lightList.front()->GetLighProjMatrix();
	//	shader->GetMatrix("g_LightView")->SetMatrix((float*)&lightView);
	//	shader->GetMatrix("g_LightProj")->SetMatrix((float*)&lightProj);
	//}
	
	// IA
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();


	shader->DrawIndexed(_techniqueIndex, _pass, _mesh->GetIndexBuffer()->GetCount(), 0, 0);

	return S_OK;
}

HRESULT MeshRenderer::RenderShadow()
{
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

	auto gameObjectWorld = GetTransform()->GetWorldMatrix();
	_shadowShader->PushTransformData(TransformDesc{ gameObjectWorld });


	//IA
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();

	_shadowShader->DrawIndexed(_techniqueIndex, _pass, _mesh->GetIndexBuffer()->GetCount(), 0, 0);


	return S_OK;
}

void MeshRenderer::OnInspectorGUI()
{
	GuiRemoveButton("MeshRenderer");

    if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();
        // Mesh 정보
        string meshName = _mesh ? Utils::ToString(_mesh->GetName()) : "None";
        ImGui::Text("Mesh: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), meshName.c_str());


		// 메쉬 변경 버튼
		if (ImGui::Button("Change Mesh##Mesh", ImVec2(-1, 0))) // 너비를 가득 채우려면 ImVec2(-1, 0)
		{
			ImGui::OpenPopup("MeshResourceSearchPopup");
		}

		// 팝업 내부 로직
		if (ImGui::BeginPopup("MeshResourceSearchPopup"))
		{
			ImGui::TextDisabled("Meshes");
			ImGui::Separator();

			const auto& resources = GAME.GetResourceArray();

			for (const auto& pair : resources[static_cast<uint8>(ResourceType::MESH)])
			{
				const auto& mesh = pair.second;
				if (ImGui::Selectable(Utils::ToString(mesh->GetName()).c_str()))
				{
					_mesh = static_pointer_cast<Mesh>(mesh);
				}
			}
			ImGui::EndPopup();
		}

        ImGui::Separator();

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


        // Material 정보
        string matName = _material ? Utils::ToString(_material->GetName()) : "None";
        ImGui::Text("Material: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.1f, 1.0f), matName.c_str());

		// Material 변경 버튼
		if (ImGui::Button("Change Material##Material", ImVec2(-1, 0))) // 너비를 가득 채우려면 ImVec2(-1, 0)
		{
			ImGui::OpenPopup("MaterialResourceSearchPopup");
		}

		// 팝업 내부 로직
		if (ImGui::BeginPopup("MaterialResourceSearchPopup"))
		{
			ImGui::TextDisabled("Materials");
			ImGui::Separator();

			const auto& resources = GAME.GetResourceArray();

			for (const auto& pair : resources[static_cast<uint8>(ResourceType::MATERIAL)])
			{
				const auto& material = pair.second;
				if (ImGui::Selectable(Utils::ToString(material->GetName()).c_str()))
				{
					_material = static_pointer_cast<Material>(material);
				}
			}
			ImGui::EndPopup();
		}

		// --- Shader 정보 ---
		if (_material == nullptr)
			return;

		string shaderName = _material->GetShader() ? Utils::ToString(_material->GetShader()->GetName()) : "None";
		ImGui::Text("Shader: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), shaderName.c_str());

		//// Shader 변경 버튼 // Material의 쉐이더 변경하면 Material 들고있는 객체들 다 쉐이더 바뀜
		//if (ImGui::Button("Change Shader##Shader", ImVec2(-1, 0))) // 너비를 가득 채우려면 ImVec2(-1, 0)
		//{
		//	ImGui::OpenPopup("ShaderResourceSearchPopup");
		//}

		//// 팝업 내부 로직
		//if (ImGui::BeginPopup("ShaderResourceSearchPopup"))
		//{
		//	ImGui::TextDisabled("Shaders");
		//	ImGui::Separator();

		//	const auto& resources = GAME.GetResourceArray();

		//	for (const auto& pair : resources[static_cast<uint8>(ResourceType::SHADER)])
		//	{
		//		const auto& shader = pair.second;
		//		if (ImGui::Selectable(Utils::ToString(shader->GetName()).c_str()))
		//		{
		//			_material->SetShader(static_pointer_cast<Shader>(shader));
		//		}
		//	}
		//	ImGui::EndPopup();
		//}

		// Shader가 가진 Technique 개수만큼 순회하며 이름을 가져옴
		uint32 techCount = _material->GetShader()->GetTechniqueCount();
		string currentTechName = _material->GetShader()->GetTechniqueName(_techniqueIndex);

		if (ImGui::BeginCombo("Technique", currentTechName.c_str()))
		{
			for (uint32 i = 0; i < techCount; ++i)
			{
				bool isSelected = (_techniqueIndex == i);
				if (ImGui::Selectable(_material->GetShader()->GetTechniqueName(i).c_str(), isSelected))
				{
					_techniqueIndex = i;
					_pass = 0; // Technique이 바뀌면 Pass는 0으로 리셋해야 안전함
				}
			}
			ImGui::EndCombo();
		}

        // Pass 설정 (uint8)
		uint32 maxPassCount = _material->GetShader()->GetPassCount(_techniqueIndex);
        int currentPass = static_cast<int>(_pass);
		// 해당 Technique이 가진 Pass 개수 내에서만 조절 가능하게 제한
		if (ImGui::SliderInt("Pass", &currentPass, 0, maxPassCount - 1))
		{
			_pass = static_cast<uint32>(currentPass);
		}

        // 추가 정보 (디버깅용)
        if (_mesh) {
            ImGui::Text("Vertex Count: %d", _mesh->GetVertexCount());
        }
    }
	ImGui::Separator();
}
