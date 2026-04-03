#pragma once
#include "Level.h"

NS_BEGIN(AssimpTool)

class Level_StaticMeshTest : public Level
{
private:
	Level_StaticMeshTest(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);

public:
	~Level_StaticMeshTest();

public:
	virtual HRESULT Initialize() override;
	virtual void Update() override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Basic(const wstring& strLayerTag);

public:
	static unique_ptr<Level_StaticMeshTest> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
};

NS_END
