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
	HRESULT Remove_GameObject(shared_ptr<GameObject> pGameObject);

	void BeginFrame();
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void EndOfFrame();


	const vector<shared_ptr<GameObject>>& Get_GameObjects() const { return _gameObjectList; }
	bool CheckGameObjectInLayer(shared_ptr<GameObject> pGameObject);

private:
	vector<shared_ptr<GameObject>>	_gameObjectList;
public:
	static unique_ptr<Layer>	Create();
};

NS_END

