#include "pch.h"
#include "GameObjectFactory.h"

GameObjectFactory::GameObjectFactory()
{
}

GameObjectFactory::~GameObjectFactory()
{
}

shared_ptr<GameObject> GameObjectFactory::CreateFromFactory(const wstring& className)
{
	auto it = _creators.find(className);
	if (it != _creators.end()) return it->second();
	return nullptr;
}

unique_ptr<GameObjectFactory> GameObjectFactory::Create()
{
	unique_ptr<GameObjectFactory> pInstance = unique_ptr<GameObjectFactory>(new GameObjectFactory);
	/*if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		return nullptr;
	}*/

	return pInstance;
}
