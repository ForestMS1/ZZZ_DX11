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
	virtual ~Camera();

	virtual void Update() override;

	void UpdateMatrix();

	void SetNear(float value) { _near = value; }
	void SetFar(float value) { _far = value; }
	void SetFOV(float value) { _fov = value; }
	void SetWidth(float value) { _width = value; }
	void SetHeight(float value) { _height = value; }

	Matrix& GetViewMatrix() { return _matView; }
	Matrix& GetProjectionMatrix() { return _matProjection; }

	void SetProjectionType(ProjectionType eType) { _projectionType = eType; }
	ProjectionType GetProjectionType() const { return _projectionType; }

	//ImGui
	void OnInspectorGUI();

private:
	Matrix _matView = Matrix::Identity;
	Matrix _matProjection = Matrix::Identity;

	float _near = 0.1f;
	float _far = 1000.f;
	float _fov = XM_PI / 4.f;
	float _width = 0.f;
	float _height = 0.f;

	ProjectionType _projectionType = ProjectionType::Perspective;

public:
	static Matrix S_MatView;
	static Matrix S_MatProjection;


public:
	shared_ptr<Prototype> Clone(void* pArg) override { return nullptr; }
};

NS_END