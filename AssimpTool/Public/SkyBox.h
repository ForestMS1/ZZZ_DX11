#pragma once
#include "GameObject.h"

NS_BEGIN(AssimpTool)

class SkyBox : public GameObject
{
public:
	SkyBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
private:
	SkyBox(const SkyBox& Prototype);

public:
	~SkyBox();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;

	//virtual HRESULT Render() override;

public:
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	static unique_ptr<SkyBox> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override;

};

NS_END

