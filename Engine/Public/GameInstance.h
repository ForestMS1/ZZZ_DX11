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
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext);
	void	Update_Engine();
	HRESULT Draw();
	void Clear_Resource(uint32 iClearLevelIndex);

public:
#pragma region TIMER_MANGER
	float Get_TimeDelta(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);
	HRESULT Add_Timer(const wstring& strTimerTag);

#pragma endregion


#pragma region LEVEL_MANAGER
	HRESULT Change_Level(uint32 iNewLevelIndex, unique_ptr<class Level> pNewLevel);
#pragma endregion

#pragma region GRAPHIC_DEVICE
	HRESULT Clear_BackBuffer_View(const Vec4* pClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

	ComPtr<ID3D11Device> GetDevice();
	ComPtr<ID3D11DeviceContext> GetDeviceContext();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag, unique_ptr<class Prototype> pPrototype);
	shared_ptr<class Prototype> Clone_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion


#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject_toLayer(uint32 iPrototypeLevelIndex, const wstring& strPrototypeTag,
		uint32 iLayerLevelIndex, const wstring& strLayerTag, void* pArg = nullptr);
#pragma endregion

private:
	unique_ptr<class Graphic_Device> _graphicDevice = { nullptr };
	unique_ptr<class Timer_Manager> _timerManager = { nullptr };
	unique_ptr<class Level_Manager> _levelManager = { nullptr };
	unique_ptr<class Prototype_Manager> _prototypeManager = { nullptr };
	unique_ptr<class Object_Manager> _objectManager = { nullptr };

public:
	void Release_Engine();
};

NS_END
