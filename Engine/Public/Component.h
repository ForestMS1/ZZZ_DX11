#pragma once
#include "Prototype.h"

NS_BEGIN(Engine)

// 엔진에서 기본으로 제공할 FixedComponent
enum class ComponentType : uint8
{
	Transform,
	MeshRenderer,
	ModelRenderer,
	SpriteRenderer,
	Camera,
	Animator,
	Collider,
	Light,
	BillBoard,
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
	//Component(const Component& rhs);
public:
	virtual ~Component();

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);

	// 생명주기 함수
	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	virtual HRESULT Render() { return S_OK; }
	virtual HRESULT RenderShadow() { return S_OK; }


	// ImGUI Inspector 창 정보
	virtual void OnInspectorGUI() {};
	// ImGuI Inspector에서 사용
	void GuiRemoveButton(string componentName = "");
	void RemoveSelf() { this->SetLifeState(LIFESTATE::REMOVE); }
	// 컴포넌트가 객체로부터 떨어질 때 컴포넌트안의 메모리 정리 기회
	virtual void	  OnDestroy() {};

public:
	// LIFESTATE Property
	LIFESTATE GetLifeState() const { return _lifeState; }
	void	  SetLifeState(LIFESTATE eLifeState) { _lifeState = eLifeState; }


	// 컴포넌트 타입 리턴
	ComponentType GetType() { return _type; }

	// 컴포넌트의 주인 게임오브젝트 리턴
	shared_ptr<GameObject>	GetGameObject();
	// 컴포넌트의 주인 게임오브젝트의 트랜스폼 리턴
	shared_ptr<Transform>	GetTransform();

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) = 0;

private:
	friend class GameObject;
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

protected:
	ComponentType _type;
	weak_ptr<GameObject> _gameObject;

	// 컴포넌트도 생명주기 가져야 함
	LIFESTATE _lifeState = LIFESTATE::NONE;

};

NS_END