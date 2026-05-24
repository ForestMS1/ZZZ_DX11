#pragma once

NS_BEGIN(Engine)
class Camera;

class ENGINE_DLL CameraManager
{
private:
	CameraManager(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext);
public:
	~CameraManager();

public:
	HRESULT Initialize(void* pArg = nullptr);

	void AddCamera(const wstring& name, shared_ptr<Camera> camera);
	void ChangeCurCamera(const wstring& name);
	shared_ptr<Camera> FindCamera(const wstring& name);
	void ClearCameraMap();


	// 특정 카메라로 얼마의 시간(duration) 동안 보간할지 설정
	void InterpolateTo(const wstring& targetName, float duration);
	// 매 프레임 호출되어 보간을 계산할 함수 (Tick 또는 Update에서 호출)
	void Update();
	bool IsInterpolating() const { return _isInterpolating; }

public:
	static unique_ptr<CameraManager> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext);

private:
	map<wstring, shared_ptr<Camera>> _cameras;
	shared_ptr<Camera> _currentCamera = nullptr; // 현재 활성화된 메인 카메라
private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _deviceContext;

	// --- 보간 상태 관리 변수들 ---
	shared_ptr<Camera> _targetCamera = nullptr;  // 목표 가상 카메라
	bool  _isInterpolating = false;
	float _accTime = 0.0f;
	float _duration = 1.0f;

	// 보간 시작 시점의 메인 카메라 상태 저장용
	Vec3 _startPos;
	Vec3 _startLook;
	float _startFov;
};

NS_END
