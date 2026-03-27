#include "pch.h"
#include "Level.h"
#include "Level_Manager.h"

Level::Level(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: _device(pDevice)
	, _deviceContext(pDeviceContext)
{
}

HRESULT Level::Initialize()
{

	return S_OK;
}

void Level::Awake()
{
}
void Level::Start()
{
}

void Level::Update()
{
}

void Level::LateUpdate()
{
}

void Level::FixedUpdate()
{
}

HRESULT Level::Render()
{
	return S_OK;
}
