#include "pch.h"
#include "TestCorinScript.h"
#include "AnimFSM.h"
#include "Transition.h"
#include "AnimState.h"
#include "NetworkView.h"
void TestCorinScript::Awake()
{
	if (auto animator = GetGameObject()->GetModelAnimator())
	{
		_fsm = animator->GetFSM();
	}
	_view = GetGameObject()->GetScript<NetworkView>();
}
void TestCorinScript::Start()
{

}

void TestCorinScript::Update()
{
	if (!_view.expired())
	{
		if (!_view.lock()->IsMine())
			return;
	}

	if (_fsm.expired())
	{
		_fsm.reset();
		return;
	}
	
	auto animFSM = _fsm.lock();

	animFSM->SetBool(L"isMove", true);

	Vec3 pos = GetTransform()->GetPosition();
	Vec3 look = GetTransform()->GetLook();
	Vec3 right = GetTransform()->GetRight();
	Vec3 up = GetTransform()->GetUp();

	if (GAME.Key_Pressing(DIK_UP))
	{
		_moveSpeed += DT * 10.f;
		if (_moveSpeed >= 5.f)
			_moveSpeed = 5.f;

		pos += look * DT * _moveSpeed;

		if (GAME.Key_Pressing(DIK_LEFT))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y -= DT * _moveSpeed;
			GetTransform()->SetLocalRotation(rotation);
		}
		if (GAME.Key_Pressing(DIK_RIGHT))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y += DT * _moveSpeed;
			GetTransform()->SetLocalRotation(rotation);
		}
	}
	else if (GAME.Key_Down(DIK_DOWN))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= XM_PI * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}
	else if (GAME.Key_Pressing(DIK_DOWN))
	{
		_moveSpeed += DT * 1.5f;
		if (_moveSpeed >= 5.f)
			_moveSpeed = 5.f;

		pos += look * DT * _moveSpeed;

		if (GAME.Key_Pressing(DIK_LEFT))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y -= DT * _moveSpeed;
			GetTransform()->SetLocalRotation(rotation);
		}
		if (GAME.Key_Pressing(DIK_RIGHT))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y += DT * _moveSpeed;
			GetTransform()->SetLocalRotation(rotation);
		}
	}
	else if (GAME.Key_Down(DIK_LEFT))
	{
		_moveSpeed = 0.f;

		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= XM_PI * 0.25f;
		GetTransform()->SetLocalRotation(rotation);
		animFSM->SetBool(L"isMove", false);
	}
	else if (GAME.Key_Down(DIK_RIGHT))
	{
		_moveSpeed = 0.f;

		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y += XM_PI * 0.25f;
		GetTransform()->SetLocalRotation(rotation);
		animFSM->SetBool(L"isMove", false);
	}
	else if(!_fsm.expired())
	{
		_moveSpeed = 0.f;
		animFSM->SetBool(L"isMove", false);
	}
		GetTransform()->SetPosition(pos);



	// АјАн
	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		if (animFSM->GetBool(L"attackNormal") == true && animFSM->GetFloat(L"attackNormalStep") == 0.f)
		{
			animFSM->SetFloat(L"attackNormalStep",1.f);
		}
		else if (animFSM->GetFloat(L"attackNormalStep") == 1.f)
		{
			animFSM->SetFloat(L"attackNormalStep", 2.f);
		}

		animFSM->SetBool(L"attackNormal", true);
	}

	if (animFSM->GetBool(L"attackNormal") && GetGameObject()->GetModelAnimator()->IsCurrentAnimFinished())
	{
		animFSM->SetBool(L"attackNormal", false);
		animFSM->SetFloat(L"attackNormalStep", 0.f);
	}

	// Evade
	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_RB))
	{
		animFSM->SetTrigger(L"evade", true);
	}
}

void TestCorinScript::OnCollisionEnter(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	collider->SetDebugColor(Colors::Red);
	_collisionCount++;
}

void TestCorinScript::OnCollisionExit(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	_collisionCount--;
	if(_collisionCount <= 0)
		collider->SetDebugColor(Colors::LimeGreen);
}

void TestCorinScript::OnDestroy()
{
	_fsm.reset();
}


unique_ptr<TestCorinScript> TestCorinScript::Create()
{
	auto pInstance = unique_ptr<TestCorinScript>(new TestCorinScript);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TestCorinScript");
		return nullptr;
	}

	return pInstance;
}


void TestCorinScript::OnInspectorGUI()
{
	GuiRemoveButton("TestCorinScript");
	if (ImGui::CollapsingHeader("TestCorinScript", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}