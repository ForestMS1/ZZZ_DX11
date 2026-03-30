#pragma once

NS_BEGIN(Engine)

class GameObject;

class Layer final
{
private:
	Layer();
public:
	~Layer();

public:
	HRESULT Add_GameObject(shared_ptr<GameObject> pGameObject);
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();

private:
	list<shared_ptr<GameObject>>	_gameObjectList;
public:
	static unique_ptr<Layer>	Create();
};

NS_END

