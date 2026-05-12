#pragma once
#include "Component.h"

NS_BEGIN(Engine)

struct CameraKeyFrame
{
	float time;
	Vec3 position;
	Vec3 lookAt;
	float fov;
};

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
	virtual void LateUpdate() override;

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

	// 카메라 액션 Play / Stop
	void Play(const wstring& actionName) { _curActionName = actionName; _isPlay = true; }
	void Stop() { _curActionName = L"";  _isPlay = false; }

	// 카메라 액션 Save / Load
	void SaveAction(const wstring& filename);
	void LoadAction(const wstring& filename);


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
	BoundingFrustum _frustum;

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
	static BoundingFrustum S_Frustum;

	// 액션~
private:
	map<wstring, vector<CameraKeyFrame>> _timeline; // [액션이름, 키프레임들]
	float _elapsedTime = 0.f;  // 재생 시 DT가 누적되는 실제 진행 시간
	float _editTime = 0.f;     // 에디터에서 슬라이더로 조절하는 편집 시점의 시간
	uint32 _curKeyFrame = 0;
	wstring _curActionName = L"";

	bool _isPlay = false;

private:
	void PlayAction();
	void AddKeyFrameAtCurrent(const wstring& actionName = L"NewAction");
	void ShowKeyFrameList(const wstring& name);

public:
	shared_ptr<Prototype> Clone(void* pArg) override { return nullptr; }
};

NS_END