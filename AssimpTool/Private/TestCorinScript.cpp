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
		_fsm.reset();
	
	if(!_fsm.expired())
		_fsm.lock()->SetBool(L"isMove", true);

	Vec3 pos = GetTransform()->GetPosition();
	Vec3 look = GetTransform()->GetLook();
	Vec3 right = GetTransform()->GetRight();
	Vec3 up = GetTransform()->GetUp();

	if (GAME.Key_Pressing(DIK_UP))
	{
		pos += look * DT * 5.f;

		if (GAME.Key_Pressing(DIK_LEFT))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y -= DT * 5.f;
			GetTransform()->SetLocalRotation(rotation);
		}
		if (GAME.Key_Pressing(DIK_RIGHT))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y += DT * 5.f;
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
		pos += look * DT * 5.f;

		if (GAME.Key_Pressing(DIK_LEFT))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y -= DT * 5.f;
			GetTransform()->SetLocalRotation(rotation);
		}
		if (GAME.Key_Pressing(DIK_RIGHT))
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y += DT * 5.f;
			GetTransform()->SetLocalRotation(rotation);
		}
	}
	else if (GAME.Key_Down(DIK_LEFT))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= XM_PI * 0.25f;
		GetTransform()->SetLocalRotation(rotation);
	}
	else if (GAME.Key_Down(DIK_RIGHT))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y += XM_PI * 0.25f;
		GetTransform()->SetLocalRotation(rotation);
	}
	else if(!_fsm.expired())
	{
		_fsm.lock()->SetBool(L"isMove", false);
	}
		GetTransform()->SetPosition(pos);



	// АјАн
	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		if (_fsm.lock()->GetBool(L"attackNormal") == true && _fsm.lock()->GetFloat(L"attackNormalStep") == 0.f)
		{
			_fsm.lock()->SetFloat(L"attackNormalStep",1.f);
		}
		else if (_fsm.lock()->GetFloat(L"attackNormalStep") == 1.f)
		{
			_fsm.lock()->SetFloat(L"attackNormalStep", 2.f);
		}

		_fsm.lock()->SetBool(L"attackNormal", true);
	}

	if (_fsm.lock()->GetBool(L"attackNormal") && GetGameObject()->GetModelAnimator()->IsCurrentAnimFinished())
	{
		_fsm.lock()->SetBool(L"attackNormal", false);
		_fsm.lock()->SetFloat(L"attackNormalStep", 0.f);
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