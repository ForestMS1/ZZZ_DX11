#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL GameInstance 
{
	DECLARE_SINGLETON(GameInstance)
private:
	GameInstance();
public:
	virtual ~GameInstance();

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device> pOutDevice, ComPtr<ID3D11DeviceContext> pOutDeviceContext);
	void	Update_Engine();
	HRESULT Draw();
	void Clear_Resource(uint32 iClearLevelIndex);

public:
#pragma region TIMER_MANGER
	float Get_TimeDelta(const wstring& strTimerTag);
	void Set_TimeDelta(const wstring& strTimerTag);
	HRESULT Ready_Timer(const wstring& strTimerTag);

#pragma endregion


#pragma region LEVEL_MANAGER
	HRESULT Chanage_Level(uint32 iNewLevelIndex, unique_ptr<class Level> pNewLevel);
#pragma endregion

#pragma region GRAPHIC_DEVICE
	HRESULT Clear_BackBuffer_View(const Vec4* pClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
#pragma endregion

private:
	unique_ptr<class Graphic_Device> _graphicDevice = { nullptr };
	unique_ptr<class Timer_Manager> _timerManager = { nullptr };
	unique_ptr<class Level_Manager> _levelManager = { nullptr };

public:
	void Release_Engine();
};

NS_END
