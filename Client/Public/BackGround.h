#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class BackGround final : public GameObject
{
private:
	BackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	BackGround(const BackGround& Prototype);

public:
	~BackGround();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual HRESULT Render() override;

public:
	static unique_ptr<BackGround> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override;

};

NS_END
