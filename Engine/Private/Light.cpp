#include "pch.h"
#include "Light.h"
#include "GameObject.h"
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
	UpdateMatrix();
}

void Light::UpdateMatrix()
{
	// 1. 조명의 방향 벡터를 가져옵니다.
	Vec3& lightDir = _lightDesc.direction;
	if (lightDir == Vec3::Zero)
		lightDir = Vec3::Backward;
	lightDir.Normalize();

	Vec3 eyePosition = GetTransform()->GetPosition();
	Vec3 focusPosition = eyePosition + lightDir;
	Vec3 upDirection = { 0,1,0 };
	

	_lightViewMatrix = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	
	_lightProjMatrix = ::XMMatrixOrthographicLH(2048.f, 2048.f, 0.1f, 5000.f);
}


void Light::OnInspectorGUI()
{
	GuiRemoveButton("Light");

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("_lightDesc");


		ImGui::InputFloat3("ambient", (float*)&_lightDesc.ambient, "%.3f");
		ImGui::InputFloat3("diffuse", (float*)&_lightDesc.diffuse, "%.3f");
		ImGui::InputFloat3("specular", (float*)&_lightDesc.specular, "%.3f");
		ImGui::InputFloat3("emissive", (float*)&_lightDesc.emissive, "%.3f");
		ImGui::SliderFloat3("direction", (float*)&_lightDesc.direction, -1, 1, "%.3f");

		ImGui::Separator();

		
	}
}
