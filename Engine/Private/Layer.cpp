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

void Layer::BeginFrame()
{
	for (auto& gameObject : _gameObjectList)
	{
		switch (gameObject->GetLifeState())
		{
		case LIFESTATE::NONE:
			gameObject->Awake();
			gameObject->SetLifeState(LIFESTATE::AWAKED);
			break;
		case LIFESTATE::AWAKED:
			gameObject->Start();
			gameObject->SetLifeState(LIFESTATE::STARTED);
			break;
		default:
			break;
		}
	}
}

void Layer::Update()
{
	for (auto& gameObject : _gameObjectList)
	{
		if(LIFESTATE::STARTED == gameObject->GetLifeState())
			gameObject->Update();
	}
}


void Layer::LateUpdate()
{
	for (auto& gameObject : _gameObjectList)
	{
		if (LIFESTATE::STARTED == gameObject->GetLifeState())
			gameObject->LateUpdate();
	}
}

void Layer::FixedUpdate()
{
	for (auto& gameObject : _gameObjectList)
	{
		if (LIFESTATE::STARTED == gameObject->GetLifeState())
			gameObject->FixedUpdate();
	}
}

unique_ptr<Layer> Layer::Create()
{
	return unique_ptr<Layer>(new Layer);
}