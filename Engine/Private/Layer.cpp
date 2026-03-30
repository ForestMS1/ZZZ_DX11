#include "pch.h"
#include "Layer.h"
#include "GameObject.h"

Layer::Layer()
{
}

Layer::~Layer()
{
}

HRESULT Layer::Add_GameObject(shared_ptr<GameObject> pGameObject)
{
	if (pGameObject == nullptr)
		return E_FAIL;

	_gameObjectList.push_back(pGameObject);

	return S_OK;
}

void Layer::Awake()
{
	for (auto& gameObject : _gameObjectList)
	{
		if (nullptr != gameObject)
			gameObject->Awake();
	}
}

void Layer::Start()
{
	for (auto& gameObject : _gameObjectList)
	{
		if (nullptr != gameObject)
			gameObject->Start();
	}
}


void Layer::Update()
{
	for (auto& gameObject : _gameObjectList)
	{
		if (nullptr != gameObject)
			gameObject->Update();
	}
}


void Layer::LateUpdate()
{
	for (auto& gameObject : _gameObjectList)
	{
		if (nullptr != gameObject)
			gameObject->LateUpdate();
	}
}

void Layer::FixedUpdate()
{
	for (auto& gameObject : _gameObjectList)
	{
		if (nullptr != gameObject)
			gameObject->FixedUpdate();
	}
}

unique_ptr<Layer> Layer::Create()
{
	return unique_ptr<Layer>(new Layer);
}