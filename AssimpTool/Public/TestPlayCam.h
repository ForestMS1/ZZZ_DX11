#pragma once
#include "GameObject.h"
class TestPlayCam : public GameObject
{
public:
	TestPlayCam(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
private:
	TestPlayCam(const TestPlayCam& Prototype);
public:
	virtual ~TestPlayCam();

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;

	virtual HRESULT Render() override;

public:
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	static unique_ptr<TestPlayCam> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override;
};