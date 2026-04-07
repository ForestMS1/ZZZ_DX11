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

	pGameObject->SetMyLayer(this);
	_gameObjectList.push_back(pGameObject);

	return S_OK;
}

HRESULT Layer::Remove_GameObject(shared_ptr<GameObject> pGameObject)
{
	if (pGameObject == nullptr)
		return E_FAIL;
	
	for (auto iter = _gameObjectList.begin(); iter != _gameObjectList.end();)
	{
		if (*iter == pGameObject)
		{
			_gameObjectList.erase(iter);
			pGameObject->SetMyLayer(nullptr);
			return S_OK;
		}
		else
		{
			++iter;
		}
	}

	return E_FAIL;
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
void Layer::EndOfFrame()
{
	// Á×Àº °´Ã¼µé »èÁ¦
	for (auto iter = _gameObjectList.begin(); iter != _gameObjectList.end();)
	{
		if ((*iter)->GetLifeState() == LIFESTATE::REMOVE)
		{
			(*iter)->GetTransform()->SetParent(nullptr);
			(*iter)->SetMyLayer(nullptr);
			iter = _gameObjectList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

bool Layer::CheckGameObjectInLayer(shared_ptr<GameObject> pGameObject)
{
	for (auto& gameObject : _gameObjectList)
	{
		if (gameObject == pGameObject)
			return true;
	}
	return false;
}

unique_ptr<Layer> Layer::Create()
{
	return unique_ptr<Layer>(new Layer);
}