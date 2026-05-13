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

	// 생명주기 함수
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

	// 월드 행렬을 새로 갱신 (자식들 까지)
	void UpdateTransform();

	// Local S R T Property (여기서 Local은 부모오브젝트 기준 자신의 좌표를 의미함)
	Vec3 GetLocalScale() { return _localScale; }
	void SetLocalScale(const Vec3& localScale) { _localScale = localScale; UpdateTransform(); }
	Vec3 GetLocalRotation() { return _localRotation; }
	void SetLocalRotation(const Vec3& localRotation) { _localRotation = localRotation; UpdateTransform(); }
	Vec3 GetLocalPosition() { return _localPosition; }
	void SetLocalPosition(const Vec3& localPosition) { _localPosition = localPosition; UpdateTransform(); }

	// Local Matrix Property
	Matrix GetLocalMatrix() { return _matLocal; }
	void SetLocalMatrix(Matrix& localMat);


	void Chase(Vec3 target, float offsetLength = 0.f, float speed = 1.f);
	// LookAt
	void LookAt(Vec3 lookAt);

	// World S R T(인게임 월드 좌표 기준 의미 (부모오브젝트 상관x))
	Vec3 GetScale() { return _scale; }
	void SetScale(const Vec3& scale);
	Vec3 GetRotation() { return _rotation; }
	void SetRotation(const Vec3& rotation);
	void SetQuaternion(const Quaternion& quat);
	Vec3 GetPosition() { return _position; }
	void SetPosition(const Vec3& position);

	// 월드 행렬의 Right, Up, Look
	Vec3 GetRight() { return _matWorld.Right(); }
	Vec3 GetUp() { return _matWorld.Up(); }
	Vec3 GetLook() { return _matWorld.Backward(); } // SimpleMath 좌표계 기준이 오른손 좌표계라서 BackWard

	// 월드 행렬 Property
	Matrix GetWorldMatrix() { return _matWorld; }
	void SetWorldMatrix(Matrix& worldMat);

	// 계층
	bool HasParent() { return _parent.lock() != nullptr; }
	shared_ptr<Transform> GetParentTransform() { return _parent.lock(); }

	//SetParent는 AddChild 함수가 호출하기 위한 용도 + nullptr로 부모 해제 용도 (해제 시 _children 벡터에서도 원소 없애는 로직 필요)
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