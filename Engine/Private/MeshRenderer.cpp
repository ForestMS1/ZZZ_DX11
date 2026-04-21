#include "pch.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

MeshRenderer::MeshRenderer() : Component(ComponentType::MeshRenderer)
{
}

MeshRenderer::~MeshRenderer()
{
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
	
	// IA
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();


	shader->DrawIndexed(_techniqueIndex, _pass, _mesh->GetIndexBuffer()->GetCount(), 0, 0);

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

        // 메쉬 변경 버튼 - 리소스 매니저와 연동 필요
        if (ImGui::Button("Change Mesh##Mesh")) {
            // 리소스 선택 팝업 등을 띄우는 로직
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

		// --- Shader 정보 ---
		string shaderName = _material->GetShader() ? Utils::ToString(_material->GetShader()->GetName()) : "None";
		ImGui::Text("Shader: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), shaderName.c_str());

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
