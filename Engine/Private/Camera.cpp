#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "GameInstance.h"
#include "GameObject.h"
#include <format>
#include <filesystem>
#include <fstream>
Matrix Camera::S_MatView = Matrix::Identity;
Matrix Camera::S_MatProjection = Matrix::Identity;
BoundingFrustum Camera::S_Frustum;

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
	if(_isPlay)
		PlayAction();

	UpdateMatrix();

}

void Camera::LateUpdate()
{
	BoundingFrustum::CreateFromMatrix(_frustum, _matProjection);
	_frustum.Transform(_frustum, _matView.Invert());

	if (_isActive)
		S_Frustum = _frustum;
}

void Camera::PlayAction()
{
	auto iter = _timeline.find(_curActionName);
	if (iter == _timeline.end() || iter->second.size() < 2) return;


	const auto& keyFrames = iter->second;

	_elapsedTime += DT;

	// 트랜스폼 캐싱
	auto transform = GetTransform();

	// 마지막 키프레임 도달
	if (_elapsedTime >= keyFrames.back().time)
	{
		transform->SetLocalPosition(keyFrames.back().position);
		transform->LookAt(keyFrames.back().lookAt);
		_fov = keyFrames.back().fov;

		_elapsedTime = 0.f;
		_curKeyFrame = 0;
		_isPlay = false;

		return;
	}

	// 현재 시간에 맞는 키프레임 인덱스 갱신
	// _curKeyFrame은 현재 시간보다 바로 앞선(작거나 같은) 프레임을 가리키게 유지
	while (_curKeyFrame + 1 < keyFrames.size() && 
		keyFrames[_curKeyFrame + 1].time <= _elapsedTime)
	{
		_curKeyFrame++;
	}

	int n = (int)keyFrames.size();
	int i = _curKeyFrame;

	// 4개 제어점 인덱스 계산
	int i0 = max(0, i - 1);
	int i1 = i;
	int i2 = min(n - 1, i + 1);
	int i3 = min(n - 1, i + 2);

	const auto& f0 = keyFrames[i0];
	const auto& f1 = keyFrames[i1];
	const auto& f2 = keyFrames[i2];
	const auto& f3 = keyFrames[i3];

	// 보간
	float duration = f2.time - f1.time;
	float t = (duration > 0.f) ? (_elapsedTime - f1.time) / duration : 0.f;
	t = std::clamp(t, 0.f, 1.f);

	// 위치보간
	Vec3 lerpPos = Vec3::CatmullRom(f0.position, f1.position, f2.position, f3.position, t);
	transform->SetLocalPosition(lerpPos);

	// 시선보간
	Vec3 lerpLookAt = Vec3::CatmullRom(f0.lookAt, f1.lookAt, f2.lookAt, f3.lookAt, t);
	transform->LookAt(lerpLookAt);
	

	// fov보간
	_fov = lerp(f1.fov, f2.fov, t);
}

void Camera::UpdateMatrix()
{
	//Vec3 eyePosition = GetTransform()->GetPosition();
	//Vec3 focusPosition = eyePosition + GetTransform()->GetLook();
	//Vec3 upDirection = GetTransform()->GetUp();
	//
	//_matView = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);


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

void Camera::SaveAction(const wstring& filename)
{

	filesystem::path filePath = L"../../Saved/CameraAction";

	if (!filesystem::exists(filePath)) 
	{
		filesystem::create_directories(filePath);
	}

	filesystem::path savePath = filePath / filename;
	if (savePath.extension().empty()) savePath.replace_extension(L".bin");

	std::ofstream outFile(savePath, std::ios::binary);
	if (!outFile.is_open()) return;

	// 저장할 액션의 개수
	uint32 timelineCount = static_cast<uint32>(_timeline.size());
	outFile.write(reinterpret_cast<const char*>(&timelineCount), sizeof(uint32));

	for (auto& pair : _timeline)
	{
		// 액션 이름 (wstring) 저장
		const wstring& name = pair.first;
		uint32 nameLength = static_cast<uint32>(name.size());
		outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(uint32));
		outFile.write(reinterpret_cast<const char*>(name.data()), nameLength * sizeof(wchar_t));

		// 키프레임 개수 저장
		const auto& frames = pair.second;
		uint32 frameCount = static_cast<uint32>(frames.size());
		outFile.write(reinterpret_cast<const char*>(&frameCount), sizeof(uint32));

		// 키프레임 배열 통째로 저장 (구조체에 포인터가 없어야 가능)
		if (frameCount > 0)
		{
			outFile.write(reinterpret_cast<const char*>(frames.data()), frameCount * sizeof(CameraKeyFrame));
		}
	}

	outFile.close();
}

void Camera::LoadAction(const wstring& filename)
{
	filesystem::path filePath = L"../../Saved/CameraAction";
	filesystem::path savePath = filePath / filename;
	if (savePath.extension().empty()) savePath.replace_extension(L".bin");

	std::ifstream ifs(savePath, std::ios::binary);
	if (!ifs.is_open()) return;

	//_timeline.clear();

	// 액션 개수 읽기
	uint32 timelineCount = 0;
	ifs.read(reinterpret_cast<char*>(&timelineCount), sizeof(uint32));

	for (uint32 i = 0; i < timelineCount; ++i)
	{
		// 액션 이름 읽기
		uint32 nameLength = 0;
		ifs.read(reinterpret_cast<char*>(&nameLength), sizeof(uint32));

		wstring name;
		name.resize(nameLength);
		ifs.read(reinterpret_cast<char*>(&name[0]), nameLength * sizeof(wchar_t));

		// 키프레임 개수 읽기
		uint32 frameCount = 0;
		ifs.read(reinterpret_cast<char*>(&frameCount), sizeof(uint32));

		// 키프레임 데이터 읽기
		vector<CameraKeyFrame> frames(frameCount);
		if (frameCount > 0)
		{
			ifs.read(reinterpret_cast<char*>(frames.data()), frameCount * sizeof(CameraKeyFrame));
		}

		_timeline[name] = std::move(frames);
	}

	ifs.close();
}

//ImGui
void Camera::OnInspectorGUI()
{
	GuiRemoveButton("Camera");

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


		// -----------------------------------------------------키 프레임 에디터-------------------------------------
		static char actionInput[256] = "NewAction";
		ImGui::InputText("Action Name", actionInput, IM_ARRAYSIZE(actionInput));

		_curActionName = Utils::ToWString(actionInput);

		if (ImGui::Button("AddAction"))
		{
			_timeline[_curActionName];
		}

		ImGui::DragFloat("ElapsedTime", &_elapsedTime, 0.1f, 100.f);

		ImGui::Separator();

		if (ImGui::Button("Play"))
		{
			_isPlay = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			_isPlay = false;
			_curActionName = L"";
			_elapsedTime = 0.f;
			_curKeyFrame = 0;
		}

		if (ImGui::Button("Save"))
		{
			SaveAction(_curActionName);
		}

		static char LoadNameInput[256] = "LoadName";
		ImGui::InputText("Load Action Name", LoadNameInput, IM_ARRAYSIZE(actionInput));
		if (ImGui::Button("Load"))
		{
			LoadAction(Utils::ToWString(LoadNameInput));
		}

		//ImGui::SliderFloat("EditTime", &_editTime, 0.0f, 100.f);
		ImGui::DragFloat("EditTime", &_editTime, 0.5f, 0.0f, 100.f, "%.2f");
		if (ImGui::Button("Capture KeyFrame"))
		{
			AddKeyFrameAtCurrent(_curActionName);
		}

		// 키프레임 리스트 출력 및 수정
		ShowKeyFrameList(_curActionName);
	}
	ImGui::Separator();
}

void Camera::AddKeyFrameAtCurrent(const wstring& actionName)
{
	CameraKeyFrame n;
	n.time = _editTime;
	n.position = GetTransform()->GetPosition();
	n.lookAt = n.position + GetTransform()->GetLook();
	n.fov = _fov;

	auto& frames = _timeline[actionName];
	frames.push_back(n);

	// 시간 순서대로 정렬
	std::sort(frames.begin(), frames.end(), [](const CameraKeyFrame& a, const CameraKeyFrame& b) {
		return a.time < b.time;
		});
}

void Camera::ShowKeyFrameList(const wstring& name)
{
	auto transform = GetTransform();

	Vec3 curPos = transform->GetLocalPosition();
	Vec3 curLook = transform->GetLook();

	ImGui::DragFloat3("Pos", (float*)&curPos, 0.1f);
	ImGui::DragFloat3("LookAt", (float*)&curLook, 0.1f);

	auto& frames = _timeline[name];
	for (int i = 0; i < frames.size(); ++i)
	{
		string label = std::format("KeyFrame {} ({:.2f}s)", i, frames[i].time);

		if (ImGui::TreeNode(label.c_str()))
		{
			ImGui::DragFloat("Time", &frames[i].time, 0.1f);
			ImGui::DragFloat3("Pos", (float*)&frames[i].position, 0.1f);
			ImGui::DragFloat3("LookAt", (float*)&frames[i].lookAt, 0.1f);
			ImGui::DragFloat("FOV", &frames[i].fov, 0.5f);

			if (ImGui::Button("Delete")) {
				frames.erase(frames.begin() + i);
			}
			ImGui::TreePop();
		}
	}
}