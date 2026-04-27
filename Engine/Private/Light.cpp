#include "pch.h"
#include "Light.h"

Light::Light() 
	: Component(ComponentType::Light)
{
}

Light::~Light()
{
}

HRESULT Light::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Light::Initialize(void* pArg)
{
	return S_OK;
}

void Light::Update()
{
	GAME.Add_LightObject(SHARED_THIS(Light));
}

void Light::OnInspectorGUI()
{
	GuiRemoveButton("Light");

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("_lightDesc");
		ImGui::SliderFloat4("ambient", (float*)&_lightDesc.ambient, 0, 1, "%.3f");
		ImGui::SliderFloat4("diffuse", (float*)&_lightDesc.diffuse, 0, 1, "%.3f");
		ImGui::SliderFloat4("specular", (float*)&_lightDesc.specular, 0, 1, "%.3f");
		ImGui::SliderFloat4("emissive", (float*)&_lightDesc.emissive, 0, 1, "%.3f");
		ImGui::SliderFloat3("direction", (float*)&_lightDesc.direction, 0, 1, "%.3f");

		ImGui::Separator();

		
	}
}
