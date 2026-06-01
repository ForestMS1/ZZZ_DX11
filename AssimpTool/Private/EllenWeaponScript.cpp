#include "pch.h"
#include "EllenWeaponScript.h"
#include "EllenStateMachineScript.h"
#include "MonsterScript.h"
void EllenWeaponScript::Awake()
{
	_transformCom = GetTransform();

	if (_transformCom->GetParentTransform() == nullptr)
	{
		auto corin = GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"Ellen");
		corin->GetTransform()->AddChild(_transformCom);
	}


	_modelAnimator = _transformCom->GetParentTransform()->GetGameObject()->GetModelAnimator();
	_model = _modelAnimator->GetModel();

	if (GetGameObject()->GetCollider() == nullptr)
	{
		shared_ptr<OBBCollider> collider = make_shared<OBBCollider>();
		auto& colliderBox = collider->GetBoundingOrientedBox();

		//colliderBox.CreateFromBoundingBox(colliderBox, _model->GetMeshByName(L"Ellen_Weapon")->boundingBox);
		//colliderBox.CreateFromPoints()
		collider->SetScale(Vec3(0.019f, 0.219f, 0.922f));
		collider->SetActive(true);
		GetGameObject()->AddComponent(collider);
	}
}
void EllenWeaponScript::Start()
{

}

void EllenWeaponScript::Update()
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
	weaponOffsetMatrix *= Matrix::Identity;
	weaponOffsetMatrix *= Matrix::CreateFromQuaternion(quat);
	weaponOffsetMatrix *= Matrix::CreateTranslation(finalPosition);

	_transformCom->SetLocalMatrix(weaponOffsetMatrix);
}

void EllenWeaponScript::LateUpdate()
{
	if (auto stateMachine = _transformCom->GetParentTransform()->GetGameObject()->GetScript<EllenStateMachineScript>())
	{
		string curStateName = stateMachine->GetCurStateName();
		auto collider = GetGameObject()->GetCollider();
		if (curStateName == "EllenNormalAttack")
		{
			collider->SetActive(true);
		}
		else
		{
			collider->SetActive(false);
		}
	}
}

void EllenWeaponScript::OnCollisionEnter(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	collider->SetDebugColor(Colors::Red);
	_collisionCount++;
}

void EllenWeaponScript::OnCollisionStay(const Collision& collision)
{
	shared_ptr<MonsterScript> monsterScript;
	if (monsterScript = collision.colliderA.lock()->GetGameObject()->GetScript<MonsterScript>())
	{
		monsterScript->OnHit(3000.f);
	}
	else if (monsterScript = collision.colliderB.lock()->GetGameObject()->GetScript<MonsterScript>())
	{
		monsterScript->OnHit(3000.f);
	}
}

void EllenWeaponScript::OnCollisionExit(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	_collisionCount--;
	if (_collisionCount <= 0)
		collider->SetDebugColor(Colors::LimeGreen);
}

void EllenWeaponScript::OnDestroy()
{

}


unique_ptr<EllenWeaponScript> EllenWeaponScript::Create()
{
	auto pInstance = unique_ptr<EllenWeaponScript>(new EllenWeaponScript);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : EllenWeaponScript");
		return nullptr;
	}

	return pInstance;
}


void EllenWeaponScript::OnInspectorGUI()
{
	GuiRemoveButton("EllenWeaponScript");
	if (ImGui::CollapsingHeader("EllenWeaponScript", ImGuiTreeNodeFlags_DefaultOpen))
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