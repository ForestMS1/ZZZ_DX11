#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class MainCam : public GameObject
{
private:
	MainCam(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	MainCam(const MainCam& Prototype);
public:
	virtual ~MainCam();

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual HRESULT Render() override;

public:
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	static unique_ptr<MainCam> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override;
};

NS_END