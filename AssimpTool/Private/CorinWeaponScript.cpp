#include "pch.h"
#include "CorinWeaponScript.h"

void CorinWeaponScript::Awake()
{
	_transformCom = GetTransform();

	if (_transformCom->GetParentTransform() == nullptr)
	{
		auto corin = GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"Corin");
		corin->GetTransform()->AddChild(_transformCom);
	}


	_modelAnimator = _transformCom->GetParentTransform()->GetGameObject()->GetModelAnimator();
	_model = _modelAnimator->GetModel();

	if (GetGameObject()->GetCollider() == nullptr)
	{
		shared_ptr<OBBCollider> collider = make_shared<OBBCollider>();
		auto& colliderBox = collider->GetBoundingOrientedBox();

		colliderBox.CreateFromBoundingBox(colliderBox, _model->GetMeshByName(L"Corin_Weapon_01")->boundingBox);
		GetGameObject()->AddComponent(collider);
	}
}
void CorinWeaponScript::Start()
{

}

void CorinWeaponScript::Update()
{
	const auto& tweenDesc = _modelAnimator->GetTweenDesc();
	auto curAnimIndex = tweenDesc.curr.animIndex;
	auto curAnimframe = tweenDesc.curr.currFrame;

	const auto& animBoneTransforms = _modelAnimator->GetAnimTransforms();
	uint32 boneIndex = _model->GetBoneIndexByName(L"Bip001 Prop1");

	// 애니메이터에서 완성한 Bone의 SRT 가져옴
	Matrix weaponMatrix = animBoneTransforms[curAnimIndex].transforms[curAnimframe][boneIndex];

	Vec3 scale, position;
	Quaternion quat;
	weaponMatrix.Decompose(scale, quat, position);

	Vec3 finalPosition = position + Vec3::Transform(_offset, quat);

	Matrix weaponOffsetMatrix = Matrix::Identity;
	weaponOffsetMatrix *= Matrix::CreateScale(scale);
	weaponOffsetMatrix *= Matrix::CreateFromQuaternion(quat);
	weaponOffsetMatrix *= Matrix::CreateTranslation(finalPosition);

	_transformCom->SetLocalMatrix(weaponOffsetMatrix);
}

void CorinWeaponScript::OnCollisionEnter(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	collider->SetDebugColor(Colors::Red);
	_collisionCount++;
}

void CorinWeaponScript::OnCollisionExit(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	_collisionCount--;
	if (_collisionCount <= 0)
		collider->SetDebugColor(Colors::LimeGreen);
}

void CorinWeaponScript::OnDestroy()
{

}


unique_ptr<CorinWeaponScript> CorinWeaponScript::Create()
{
	auto pInstance = unique_ptr<CorinWeaponScript>(new CorinWeaponScript);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CorinWeaponScript");
		return nullptr;
	}

	return pInstance;
}


void CorinWeaponScript::OnInspectorGUI()
{
	GuiRemoveButton("CorinWeaponScript");
	if (ImGui::CollapsingHeader("CorinWeaponScript", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Offset
		float offset[3] = { _offset.x, _offset.y, _offset.z };
		if (ImGui::DragFloat3("Offset", offset, 0.1f, -500.f, 500.f))
		{
			_offset = { offset[0],
						offset[1],
						offset[2] };
		}

	}
}