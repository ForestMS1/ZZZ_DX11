#pragma once
#include "GameObject.h"

NS_BEGIN(AssimpTool)

class TestCorin : public GameObject
{
public:
	TestCorin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
private:
	TestCorin(const TestCorin& Prototype);

public:
	~TestCorin();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;

	//virtual HRESULT Render() override;

public:
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	static unique_ptr<TestCorin> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override;

};

NS_END

