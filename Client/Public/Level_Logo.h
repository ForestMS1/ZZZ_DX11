#pragma once

#include "Level.h"
NS_BEGIN(Client)

class Level_Logo : public Level
{
private:
	Level_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);

public:
	~Level_Logo();

public:
	virtual HRESULT Initialize() override;
	virtual void Update() override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Basic(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
public:
	static unique_ptr<Level_Logo> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);

};

NS_END
