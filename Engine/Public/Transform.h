#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Transform final : public Component
{
private:
	Transform();
public:
	virtual ~Transform();

	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg = nullptr) override;

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

	void UpdateTransform();

	//Local
	Vec3 GetLocalScale() { return _localScale; }
	void SetLocalScale(const Vec3& localScale) { _localScale = localScale; UpdateTransform(); }
	Vec3 GetLocalRotation() { return _localRotation; }
	void SetLocalRotation(const Vec3& localRotation) { _localRotation = localRotation; UpdateTransform(); }
	Vec3 GetLocalPosition() { return _localPosition; }
	void SetLocalPosition(const Vec3& localPosition) { _localPosition = localPosition; UpdateTransform(); }

	Matrix GetLocalMatrix() { return _matLocal; }
	void SetLocalMatrix(Matrix& localMat);


	void Chase(Vec3 target, float offsetLength = 0.f, float speed = 1.f);
	// LookAt
	void LookAt(Vec3 lookAt);

	// World
	Vec3 GetScale() { return _scale; }
	void SetScale(const Vec3& scale);
	Vec3 GetRotation() { return _rotation; }
	void SetRotation(const Vec3& rotation);
	Vec3 GetPosition() { return _position; }
	void SetPosition(const Vec3& position);

	Vec3 GetRight() { return _matWorld.Right(); }
	Vec3 GetUp() { return _matWorld.Up(); }
	Vec3 GetLook() { return _matWorld.Backward(); }

	Matrix GetWorldMatrix() { return _matWorld; }
	void SetWorldMatrix(Matrix& worldMat);

	// 계층
	bool HasParent() { return _parent.lock() != nullptr; }

	shared_ptr<Transform> GetParentTransform() { return _parent.lock(); }

	//SetParent는 AddChild가 호출하기 위한 용도. 외부에서 사용하지마셈.
	void SetParent(shared_ptr<Transform> pParentTransform);

	vector<shared_ptr<Transform>>& GetChildrenTransform() { return _children; }
	void AddChild(shared_ptr<Transform> pChildTransform) { pChildTransform->SetParent(SHARED_THIS(Transform)); _children.push_back(pChildTransform); }

	static Vec3 ToEulerAngles(Quaternion q);

private:
	void SetRight(const Vec3& v) { _matWorld.Right(v); }
	void SetUp(const Vec3& v) { _matWorld.Up(v); }
	void SetLook(const Vec3& v) { _matWorld.Backward(v); }
private:
	Vec3 _localScale = { 1.f, 1.f, 1.f };
	Vec3 _localRotation = { 0.f, 0.f, 0.f };
	Vec3 _localPosition = { 0.f, 0.f, 0.f };

	Matrix _matLocal = Matrix::Identity;
	Matrix _matWorld = Matrix::Identity;

	Vec3 _scale;
	Vec3 _rotation;
	Vec3 _position;

private:
	// 순환 참조 메모리 누수 때문에 weak_ptr로 변경
	weak_ptr<Transform> _parent;
	vector<shared_ptr<Transform>> _children;

public:
	static shared_ptr<Transform> Create();
	shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }
};

NS_END