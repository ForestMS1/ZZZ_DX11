#include "pch.h"
#include "Transform.h"

Transform::Transform()
	: Component(ComponentType::Transform)
{
}

Transform::~Transform()
{
}

HRESULT Transform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Transform::Initialize(void* pArg)
{
	return S_OK;
}

void Transform::Awake()
{
}

void Transform::Start()
{
}

void Transform::Update()
{
}

void Transform::LateUpdate()
{
}

void Transform::FixedUpdate()
{
}

Vec3 Transform::ToEulerAngles(Quaternion q)
{
	Vec3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
	double cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
	angles.y = 2 * std::atan2(sinp, cosp) - 3.14159f / 2;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}

void Transform::UpdateTransform()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	_matLocal = matScale * matRotation * matTranslation;

	if (HasParent())
	{
		_matWorld = _matLocal * _parent.lock()->GetWorldMatrix();
	}
	else
	{
		_matWorld = _matLocal;
	}

	Quaternion quat;
	_matWorld.Decompose(_scale, quat, _position);
	_rotation = ToEulerAngles(quat);

	for (auto& childTransform : _children)
		childTransform->UpdateTransform();

}

void Transform::SetLocalMatrix(Matrix& localMat)
{
	// 내부 로컬 행렬 변수를 전달받은 행렬로 설정
	_matLocal = localMat;

	// 행렬로부터 Scale, Rotation(Quaternion), Translation 성분을 분리(Decompose)
	Quaternion qRotation;
	if (_matLocal.Decompose(_localScale, qRotation, _localPosition))
	{
		// 쿼터니언 형태의 회전값을 오일러 각(Euler Angles)으로 변환하여 저장
		_localRotation = ToEulerAngles(qRotation);
	}

	// 로컬 정보가 바뀌었으므로 월드 행렬을 다시 계산
	UpdateTransform();
}

void Transform::Chase(Vec3 targetWorldPos, float offsetLength, float speed)
{
	Vec3 worldPos = GetPosition();

	Vec3 forward = targetWorldPos - worldPos;
	if (forward.LengthSquared() < offsetLength) return;
	forward.Normalize();

	LookAt(targetWorldPos);

	worldPos += forward * speed * DT;

	SetLocalPosition(worldPos);
}

void Transform::LookAt(Vec3 targetWorldPos)
{
	Vec3 worldPos = GetPosition();

	//  방향 벡터 계산 (타겟 - 현재위치)
	Vec3 newLook = targetWorldPos - worldPos;
	if (newLook.LengthSquared() < 0.001f) return;
	newLook.Normalize();

	Vec3 newRight = ::XMVector3Cross(Vec3(0.f, 1.f, 0.f), newLook);
	Vec3 newUp = ::XMVector3Cross(newLook, newRight);
	newRight.Normalize();
	newUp.Normalize();


	SetRight(newRight * GetLocalScale().x);
	SetUp(newUp * GetLocalScale().y);
	SetLook(newLook * GetLocalScale().z);
}

void Transform::SetScale(const Vec3& worldScale)
{
	if (HasParent())
	{
		Vec3 parentScale = _parent.lock()->GetScale();
		Vec3 scale = worldScale;
		scale.x /= parentScale.x;
		scale.y /= parentScale.y;
		scale.z /= parentScale.z;
		SetLocalScale(scale);
	}
	else
	{
		SetLocalScale(worldScale);
	}
}

void Transform::SetRotation(const Vec3& worldRotation)
{
	if (HasParent())
	{
		Matrix InverseMatrix = _parent.lock()->GetWorldMatrix().Invert();

		Vec3 rotation;
		rotation.TransformNormal(worldRotation, InverseMatrix);

		SetLocalRotation(rotation);
	}
	else
	{
		SetLocalRotation(worldRotation);
	}
}

void Transform::SetPosition(const Vec3& worldPosition)
{
	if (HasParent())
	{
		Matrix worldToParentLocalMatrix = _parent.lock()->GetWorldMatrix().Invert();

		Vec3 position;
		position.Transform(worldPosition, worldToParentLocalMatrix);

		SetLocalPosition(position);
	}
	else
	{
		SetLocalPosition(worldPosition);
	}
}

void Transform::SetWorldMatrix(Matrix& worldMat)
{
	// 전달받은 월드 행렬을 Decompose 하여 월드 기준 Pos, Rot, Scale 추출
	Vec3 worldPos, worldScale;
	Quaternion worldQuat;

	worldMat.Decompose(worldScale, worldQuat, worldPos);


	// 부모가 있다면 월드 값을 로컬 값으로 변환
	if (HasParent())
	{
		// 부모의 월드 행렬 역행렬 계산
		Matrix invParentWorld = _parent.lock()->GetWorldMatrix().Invert();

		// 위치 변환 (World -> Parent Local)
		_localPosition = Vec3::Transform(worldPos, invParentWorld);

		// 크기 변환 (부모의 스케일 영향을 제거)
		Vec3 parentScale = _parent.lock()->GetScale();
		_localScale = worldScale;
		_localScale.x /= parentScale.x;
		_localScale.y /= parentScale.y;
		_localScale.z /= parentScale.z;

		// 회전 변환 (부모의 회전 영향을 제거)
		// 월드 쿼터니언 = 로컬 쿼터니언 * 부모 월드 쿼터니언
		// 로컬 쿼터니언 = 월드 쿼터니언 * 부모 월드 쿼터니언의 역행렬
		Vec3 dummyS, dummyP;
		Quaternion parentQuat;
		_parent.lock()->GetWorldMatrix().Decompose(dummyS, parentQuat, dummyP);

		Quaternion invParentQuat;
		parentQuat.Inverse(invParentQuat);
		Quaternion localQuat = worldQuat * invParentQuat;
		_localRotation = ToEulerAngles(localQuat);
	}
	else
	{
		// 부모가 없으면 월드 값이 곧 로컬 값
		_localPosition = worldPos;
		_localScale = worldScale;
		_localRotation = ToEulerAngles(worldQuat);
	}

	// 변경된 로컬 값들을 바탕으로 다시 행렬 전체 업데이트 (자식들까지)
	UpdateTransform();
}

void Transform::SetParent(shared_ptr<Transform> pParentTransform)
{
	if (_parent.lock() == pParentTransform)
		return;

	if (HasParent())
	{
		vector<shared_ptr<Transform>>& childVec = _parent.lock()->GetChildrenTransform();
		for (size_t i = 0; i < childVec.size(); )
		{
			if (childVec[i] == SHARED_THIS(Transform))
			{
				childVec.erase(childVec.begin() + i);
			}
			else
				++i;
		}
	}
	_parent = pParentTransform;
}

shared_ptr<Transform> Transform::Create()
{
	auto		pInstance = shared_ptr<Transform>(new Transform);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Transform");
		return nullptr;
	}

	return pInstance;
}