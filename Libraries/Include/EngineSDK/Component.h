#pragma once
#include "Prototype.h"

NS_BEGIN(Engine)

enum class ComponentType : uint8
{
	Transform,
	MeshRenderer,
	ModelRenderer,
	Camera,
	Animator,
	//...
	Script,

	End,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(ComponentType::End) - 1
};

class GameObject;
class Transform;

class ENGINE_DLL Component abstract: public Prototype
{
protected:
	Component(ComponentType eType);
public:
	virtual ~Component();

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}

public:
	ComponentType GetType() { return _type; }

	shared_ptr<GameObject>	GetGameObject();
	shared_ptr<Transform>	GetTransform();

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) = 0;

private:
	friend class GameObject;
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

protected:
	ComponentType _type;
	weak_ptr<GameObject> _gameObject;

};

NS_END