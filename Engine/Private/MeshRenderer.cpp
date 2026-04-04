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
	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();


	shader->DrawIndexed(0, _pass, _mesh->GetIndexBuffer()->GetCount(), 0, 0);

	return S_OK;
}

void MeshRenderer::OnInspectorGUI()
{
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

    // Material 정보
    string matName = _material ? Utils::ToString(_material->GetName()) : "None";
    ImGui::Text("Material: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.1f, 1.0f), matName.c_str());

    // Pass 설정 (uint8)
    int currentPass = static_cast<int>(_pass);
    if (ImGui::SliderInt("Render Pass", &currentPass, 0, 10)) // 최대 패스 범위는 엔진 설계에 맞게
    {
        _pass = static_cast<uint8>(currentPass);
    }

    // 추가 정보 (디버깅용)
    if (_mesh) {
        ImGui::Text("Vertex Count: %d", _mesh->GetVertexCount());
    }
}
