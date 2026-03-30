#include "pch.h"
#include "Prototype_Manager.h"
#include "GameObject.h"
#include "Component.h"

Prototype_Manager::Prototype_Manager()
{
}

HRESULT Prototype_Manager::Initialize(uint32 iNumLevels)
{
	_protoTypes = unique_ptr<PROTOTYPES[]>(new PROTOTYPES[iNumLevels]);

	_numLevels = iNumLevels;
	
	return S_OK;
}

HRESULT Prototype_Manager::Add_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag, unique_ptr<Prototype> pPrototype)
{
	if (nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
		return E_FAIL;

	_protoTypes[iLevelIndex].emplace(strPrototypeTag, std::move(pPrototype));

	return S_OK;
}

shared_ptr<Prototype> Prototype_Manager::Clone_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	Prototype* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (pPrototype == nullptr)
		return nullptr;

	return pPrototype->Clone(pArg);
}

Prototype* Prototype_Manager::Find_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag)
{
	if (iLevelIndex >= _numLevels)
		return nullptr;

	auto	iter = _protoTypes[iLevelIndex].find(strPrototypeTag);
	if (iter == _protoTypes[iLevelIndex].end())
		return nullptr;

	return iter->second.get();
}

unique_ptr<Prototype_Manager> Prototype_Manager::Create(uint32 iNumLevels)
{
	auto pInstance = unique_ptr<Prototype_Manager>(new Prototype_Manager);
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : Prototype_Manager");
		return nullptr;
	}

	return pInstance;
}
