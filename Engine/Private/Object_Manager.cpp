#include "pch.h"
#include "Object_Manager.h"
#include "Layer.h"

#include "GameInstance.h"
#include "GameObject.h"

Object_Manager::Object_Manager()
{
}

Object_Manager::~Object_Manager()
{
}

HRESULT Object_Manager::Initialize(uint32 iNumLevels)
{
	_layerMaps = unique_ptr<LAYERS[]>(new LAYERS[iNumLevels]);


	_numLevels = iNumLevels;

	return S_OK;
}


void Object_Manager::BeginFrame()
{
	_currentLevelIndex = GameInstance::Get().GetCurrentLevelIndex();

	for (auto& pair : _layerMaps[_currentLevelIndex])
	{
		pair.second->BeginFrame();
	}

}

void Object_Manager::Update()
{
	for (auto& pair : _layerMaps[_currentLevelIndex])
	{
		pair.second->Update();
	}
}

void Object_Manager::LateUpdate()
{
	for (auto& pair : _layerMaps[_currentLevelIndex])
	{
		pair.second->LateUpdate();
	}
}

void Object_Manager::FixedUpdate()
{
	for (auto& pair : _layerMaps[_currentLevelIndex])
	{
		pair.second->FixedUpdate();
	}
}

HRESULT Object_Manager::Add_GameObject_toLayer(uint32 iPrototypeLevelIndex, const wstring& strPrototypeTag, uint32 iLayerLevelIndex, const wstring& strLayerTag, void* pArg)
{
 	if (nullptr == _layerMaps || iLayerLevelIndex >= _numLevels)
		return E_FAIL;

	shared_ptr<GameObject> pGameObject = dynamic_pointer_cast<GameObject>(GameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag));
	if (nullptr == pGameObject)
		return E_FAIL;

	// 현재 객체를 추가하려고 하는 레이어가 없다면
	// 새로 레이어를 만들어 추가해준다.
	Layer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		unique_ptr<Layer> pNewLayer = Layer::Create();
		if (nullptr == pNewLayer)
			return E_FAIL;
		
		if (FAILED(pNewLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		_layerMaps[iLayerLevelIndex].emplace(strLayerTag, std::move(pNewLayer));
	}
	else
	{
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;
	}


	return S_OK;
}

void Object_Manager::Clear(uint32 iClearLevelIndex)
{
	if (iClearLevelIndex >= _numLevels ||
		nullptr == _layerMaps)
	{
		MSG_BOX("Object_Manager Clear Failed!");
		return;
	}

	for (auto& pair : _layerMaps[iClearLevelIndex])
	{
		pair.second.reset();
	}
	_layerMaps[iClearLevelIndex].clear();
}



Layer* Object_Manager::Find_Layer(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	if (iLayerLevelIndex >= _numLevels)
		return nullptr;

	auto iter = _layerMaps[iLayerLevelIndex].find(strLayerTag);
	if (iter == _layerMaps[iLayerLevelIndex].end())
		return nullptr;

	return iter->second.get();
}


unique_ptr<Object_Manager> Object_Manager::Create(uint32 iNumLevels)
{
	unique_ptr<Object_Manager> pInstance = unique_ptr<Object_Manager>(new Object_Manager);
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		return nullptr;
	}

	return pInstance;
}