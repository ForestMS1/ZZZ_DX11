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
	Vec3 lightDir = _lightDesc.direction;
	lightDir.Normalize();

	Vec3 eyePosition = GetTransform()->GetLocalPosition();
	Vec3 focusPosition = eyePosition + lightDir;
	Vec3 upDirection = { 0,1,0 };
	

	_lightViewMatrix = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	
	// 가로 2000, 세로 2000, 근거리 1, 원거리 5000 범위의 직교 투영 행렬
	_lightProjMatrix = ::XMMatrixOrthographicLH(2048, 2048, 0.1f, 50000.f);
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
