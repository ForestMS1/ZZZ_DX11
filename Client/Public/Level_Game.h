#pragma once
#include "Level.h"

NS_BEGIN(Client)

class Level_Game final : public Level
{
private:
	Level_Game(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);

public:
	~Level_Game();

public:
	virtual HRESULT Initialize() override;
	virtual void Update() override;
	virtual HRESULT Render() override;

public:
	static unique_ptr<Level_Game> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);

};

NS_END
