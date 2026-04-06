#pragma once

#include "Resource_Manager.h"

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
	ENGINE_DESC GetEngineDesc() const { return _desc; }

public:
#pragma region TIMER_MANGER
	float Get_TimeDelta(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);
	HRESULT Add_Timer(const wstring& strTimerTag);

#pragma endregion


#pragma region LEVEL_MANAGER
	HRESULT Change_Level(uint32 iNewLevelIndex, unique_ptr<class Level> pNewLevel);
	uint32 GetCurrentLevelIndex();
#pragma endregion

#pragma region GRAPHIC_DEVICE
	HRESULT Clear_BackBuffer_View(const Vec4* pClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

	ComPtr<ID3D11Device> GetDevice();
	ComPtr<ID3D11DeviceContext> GetDeviceContext();
	ComPtr<IDXGISwapChain> GetSwapChain();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag, unique_ptr<class Prototype> pPrototype);
	shared_ptr<class Prototype> Clone_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion


#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject_toLayer(uint32 iPrototypeLevelIndex, const wstring& strPrototypeTag,
		uint32 iLayerLevelIndex, const wstring& strLayerTag, void* pArg = nullptr);

	shared_ptr<class GameObject> Find_GameObject_fromLayer(const wstring& strLayerTag, const wstring& objName);
	void DisableCameras();
	void firstFindCamOn();

	void ShowHiearchy();
	void ShowInspector();
	void RenderGizmo();
#pragma endregion

#pragma region RESOURCE_MANAGER
	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path);

	template<typename T>
	bool AddResource(const wstring& key, shared_ptr<T> object);

	template<typename T>
	shared_ptr<T> GetResource(const wstring& key);

	shared_ptr<Texture> GetOrAddTexture(const wstring& key, const wstring& path);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<class GameObject> pRenderObject);
#pragma endregion

#pragma region INPUT_MANAGER
	signed char	Get_DIKeyState(unsigned char byKeyID);

	signed char	Get_DIMouseState(MOUSEKEYSTATE eMouse);

	// 현재 마우스의 특정 축 좌표를 반환
	signed long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

	bool Key_Pressing(unsigned char byKeyID);
	bool Key_Up(unsigned char byKeyID);
	bool Key_Down(unsigned char byKeyID);

	bool Mouse_Pressing(MOUSEKEYSTATE eMouseState);
	bool Mouse_Up(MOUSEKEYSTATE eMouseState);
	bool Mouse_Down(MOUSEKEYSTATE eMouseState);
#pragma endregion

	void SetEngineContext(ImGuiContext* pContext);

private:
	unique_ptr<class Graphic_Device> _graphicDevice = { nullptr };
	unique_ptr<class Timer_Manager> _timerManager = { nullptr };
	unique_ptr<class Level_Manager> _levelManager = { nullptr };
	unique_ptr<class Prototype_Manager> _prototypeManager = { nullptr };
	unique_ptr<class Object_Manager> _objectManager = { nullptr };
	unique_ptr<class Renderer> _renderer = { nullptr };
	unique_ptr<Resource_Manager> _resourceManager = { nullptr };
	unique_ptr<class Input_Manager> _inputManager = { nullptr };

private:
	ENGINE_DESC _desc;

public:
	void Release_Engine();
};

#pragma region RESOURCE_MANAGER

template<typename T>
shared_ptr<T> GameInstance::Load(const wstring& key, const wstring& path)
{
	return _resourceManager->Load<T>(key, path);
}

template<typename T>
bool GameInstance::AddResource(const wstring& key, shared_ptr<T> object)
{
	return _resourceManager->AddResource<T>(key, object);
}

template<typename T>
shared_ptr<T> GameInstance::GetResource(const wstring& key)
{
	return _resourceManager->GetResource<T>(key);
}

#pragma endregion

NS_END
