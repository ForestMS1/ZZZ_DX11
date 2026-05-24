#include "pch.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Transform.h"

CameraManager::CameraManager(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
	: _device(device), _deviceContext(deviceContext)
{

}

CameraManager::~CameraManager()
{

}

HRESULT CameraManager::Initialize(void* pArg)
{
	return S_OK;
}

void CameraManager::AddCamera(const wstring& name, shared_ptr<Camera> camera)
{
	if (camera == nullptr)
		return;

	if (_cameras.empty())
	{
		camera->CameraOn();
		_currentCamera = camera;
	}
	else
	{
		camera->CameraOff();
	}
	_cameras.emplace(name, camera);
}

void CameraManager::ChangeCurCamera(const wstring& name)
{
	auto iter = _cameras.find(name);
	if (iter != _cameras.end())
	{
		if (_currentCamera != nullptr)
			_currentCamera->CameraOff();

		_currentCamera = iter->second;
		_currentCamera->CameraOn();
		_isInterpolating = false; // 즉시 변경 시 보간 중단
	}
}

shared_ptr<Camera> CameraManager::FindCamera(const wstring& name)
{
	auto iter = _cameras.find(name);
	if (iter == _cameras.end())
		return nullptr;

	return iter->second;
}

void CameraManager::ClearCameraMap()
{
	_currentCamera = nullptr;
	_cameras.clear();
}

// 특정 카메라로 얼마의 시간(duration) 동안 보간할지 설정
void CameraManager::InterpolateTo(const wstring& targetName, float duration)
{
	if (_currentCamera == nullptr) return;

	auto iter = _cameras.find(targetName);
	if (iter == _cameras.end()) return;

	_targetCamera = iter->second;
	_duration = duration;
	_accTime = 0.0f;
	_isInterpolating = true;

	// 시작 시점의 메인 카메라 데이터 백업
	auto transform = _currentCamera->GetTransform();
	_startPos = transform->GetLocalPosition();
	_startLook = transform->GetLocalLook();
	_startFov = _currentCamera->GetFov();
}

// 매 프레임 호출되어 보간을 계산할 함수 (Tick 또는 Update에서 호출)
void CameraManager::Update()
{
	if (!_isInterpolating || !_currentCamera || !_targetCamera)
		return;

	_accTime += DT;
	float t = _accTime / _duration;

	if (t >= 1.0f)
	{
		t = 1.0f;
		_isInterpolating = false;
	}

	// Position 보간 (Vector3::Lerp 제공)
	Vec3 targetPos = _targetCamera->GetTransform()->GetLocalPosition();
	Vec3 currentPos = Vec3::Lerp(_startPos, targetPos, t);

	// Look Vector 보간 
	// 두 벡터가 정반대 방향이 아니라면, Lerp 후 Normalize하는 방식(Nlerp)으로도 
	// 충분히 부드러운 카메라 회전이 연출됩니다.
	Vec3 targetLook = _targetCamera->GetTransform()->GetLocalLook();
	Vec3 currentLook = Vec3::Lerp(_startLook, targetLook, t);
	currentLook.Normalize(); // 방향 벡터 유지를 위해 필수!

	// FOV 보간 (기본 선형 보간 정석)
	float currentFov = _startFov + t * (_targetCamera->GetFov() - _startFov);

	// 메인 카메라에 값 세팅
	_currentCamera->GetTransform()->SetLocalPosition(currentPos);
	_currentCamera->GetTransform()->LookAtLocal(currentLook);
	_currentCamera->SetFOV(currentFov);


	if (!_isInterpolating)
	{
		_targetCamera = nullptr;
	}
}

unique_ptr<CameraManager> CameraManager::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
{
	auto pInstance = unique_ptr<CameraManager>(new CameraManager(device, deviceContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Cloned : CameraManager");
		return nullptr;
	}

	return pInstance;
}
