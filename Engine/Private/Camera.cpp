#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "GameInstance.h"

Matrix Camera::S_MatView = Matrix::Identity;
Matrix Camera::S_MatProjection = Matrix::Identity;

Camera::Camera()
	: Component(ComponentType::Camera)
{
	_width = static_cast<float>(GAME.GetEngineDesc().iWinSizeX);
	_height = static_cast<float>(GAME.GetEngineDesc().iWinSizeY);
}

Camera::~Camera()
{

}

void Camera::Update()
{
	UpdateMatrix();

	//PushGlobalData;
}

void Camera::UpdateMatrix()
{
	//Vec3 eyePosition = GetTransform()->GetPosition();
	//Vec3 focusPosition = eyePosition + GetTransform()->GetLook();
	//Vec3 upDirection = GetTransform()->GetUp();
	//S_MatView = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);

	_matView = GetTransform()->GetWorldMatrix().Invert();

	if(_projectionType == ProjectionType::Perspective)
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, _width / _height, _near, _far);
	else
		_matProjection = ::XMMatrixOrthographicLH(_fov, _width / _height, _near, _far);

	if (_isActive)
	{
		S_MatView = _matView;
		S_MatProjection = _matProjection;
	}
}

//ImGui
void Camera::OnInspectorGUI()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (ImGui::Button(_isActive ? "OFF" : "ON")) {
			_isActive ? CameraOff() : CameraOn();
		}

		// --- Perspective Or Orthographic ---
		string projectionTypeName = (_projectionType == ProjectionType::Perspective) ? "Perspective" : "Orthographic";
		ImGui::Text("ProjectionType: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), projectionTypeName.c_str());


		if (ImGui::BeginCombo("ProjectionType", projectionTypeName.c_str()))
		{
			for (uint32 i = 0; i < ETOUI(ProjectionType::END); ++i)
			{
				string name = i == ETOUI(ProjectionType::Perspective) ? "Perspective" : "Orthographic";
				bool isSelected = (ETOUI(_projectionType) == i);
				if (ImGui::Selectable(name.c_str(), isSelected))
				{
					_projectionType = static_cast<ProjectionType>(i);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();

		ImGui::SliderFloat("Near", &_near, 0.1f, 1.f, "%.3f"); ImGui::SameLine(); if (ImGui::Button("R##Near")) _near = 0.1f;
		ImGui::SliderFloat("Far", &_far, 0.2f, 1000.f, "%.3f"); ImGui::SameLine(); if (ImGui::Button("R##Far")) _far = 1000.f;
		ImGui::SliderFloat("Fov", &_fov, 0.1f, XM_PI, "%.3f"); ImGui::SameLine(); if (ImGui::Button("R##Fov")) _fov = XM_PI / 4.f;;
		ImGui::SliderFloat("Width", &_width, 0.1f, static_cast<float>(GAME.GetEngineDesc().iWinSizeX), "%.3f"); ImGui::SameLine(); if (ImGui::Button("R##Width")) _width = static_cast<float>(GAME.GetEngineDesc().iWinSizeX);
		ImGui::SliderFloat("Height", &_height, 0.1f, static_cast<float>(GAME.GetEngineDesc().iWinSizeY), "%.3f"); ImGui::SameLine(); if (ImGui::Button("R##Height")) _height = static_cast<float>(GAME.GetEngineDesc().iWinSizeY);
		if (ImGui::Button("ResetAll##All"))
		{
			_projectionType = ProjectionType::Perspective;
			_near = 0.1f;
			_far = 1000.f;
			_fov = XM_PI / 4.f;
			_width = static_cast<float>(GAME.GetEngineDesc().iWinSizeX);
			_height = static_cast<float>(GAME.GetEngineDesc().iWinSizeY);
		}
		ImGui::Separator();

		ImGui::Unindent();
	}
}