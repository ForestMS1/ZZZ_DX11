#pragma once
#include "Component.h"

NS_BEGIN(Engine)

enum class ProjectionType
{
	Perspective = 0, // 원근 투영
	Orthographic, // 직교 투영
	END
};

class ENGINE_DLL Camera : public Component
{
public:
	Camera();
	Camera(const Camera& rhs);
	virtual ~Camera();

public:
	// 생명주기 함수
	virtual void Update() override;

	// V, P 갱신
	void UpdateMatrix();

	// 카메라 세팅값 Set
	void SetNear(float value) { _near = value; }
	void SetFar(float value) { _far = value; }
	void SetFOV(float value) { _fov = value; }
	void SetWidth(float value) { _width = value; }
	void SetHeight(float value) { _height = value; }

	// V, P Getter
	Matrix& GetViewMatrix() { return _matView; }
	Matrix& GetProjectionMatrix() { return _matProjection; }

	// 투영 방식 Property
	void SetProjectionType(ProjectionType eType) { _projectionType = eType; }
	ProjectionType GetProjectionType() const { return _projectionType; }


	// 카메라 On/Off
	friend class Object_Manager;
	void CameraOn() { GAME.DisableCameras(); _isActive = true; }
	void CameraOff() { _isActive = false;  GAME.firstFindCamOn(); }
	bool IsActive() const { return _isActive; }

	
	//ImGui
	virtual void OnInspectorGUI() override;

private:
	Matrix _matView = Matrix::Identity;
	Matrix _matProjection = Matrix::Identity;

	float _near = 0.1f;
	float _far = 1000.f;
	float _fov = XM_PI / 4.f;
	float _width = 0.f;
	float _height = 0.f;

	ProjectionType _projectionType = ProjectionType::Perspective;

	// 카메라 On/Off
	bool _isActive = false;
public:
	static Matrix S_MatView;
	static Matrix S_MatProjection;

public:
	shared_ptr<Prototype> Clone(void* pArg) override { return nullptr; }
};

NS_END