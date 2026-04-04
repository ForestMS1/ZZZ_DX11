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

	void BeginFrame();
	void Update();
	void LateUpdate();
	void FixedUpdate();

	// ImGui에서 순회하기 위해 객체 리스트를 노출하거나, 
	// Layer 자체가 ImGui를 렌더링하는 함수를 가지게 할 수 있습니다.
	const list<shared_ptr<GameObject>>& Get_GameObjects() const { return _gameObjectList; }

private:
	list<shared_ptr<GameObject>>	_gameObjectList;
public:
	static unique_ptr<Layer>	Create();
};

NS_END

