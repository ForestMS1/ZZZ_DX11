#include "pch.h"
#include "CorinMove.h"
#include "CorinStateMachineScript.h"

CorinMove::CorinMove(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinMove::~CorinMove()
{
}

void CorinMove::OnEnter()
{
	auto stateMachine = static_pointer_cast<CorinStateMachineScript>(_stateMachine.lock());
	_animator.lock()->SetFloat(L"speed", stateMachine->_moveSpeed);
}

void CorinMove::Input()
{
	auto transform = _gameObject.lock()->GetTransform();
	auto stateMachine = static_pointer_cast<CorinStateMachineScript>(_stateMachine.lock());

	// Run_End 중 일땐 입력무시
	//if (stateMachine->GetCurAnimStateName() == L"Run_End")
	//	return;

	Vec3 pos = transform->GetPosition();
	Vec3 look = transform->GetLook();
	Vec3 right = transform->GetRight();
	Vec3 up = transform->GetUp();

	_animator.lock()->SetFloat(L"speed", stateMachine->_moveSpeed);

	if (GAME.Key_Pressing(DIK_UP))
	{
		stateMachine->_moveSpeed += DT * 10.f;
		if (stateMachine->_moveSpeed >= 5.f)
			stateMachine->_moveSpeed = 5.f;

		pos += look * DT * stateMachine->_moveSpeed;

		if (GAME.Key_Pressing(DIK_LEFT))
		{
			Vec3 rotation = transform->GetLocalRotation();
			rotation.y -= DT * stateMachine->_moveSpeed;
			transform->SetLocalRotation(rotation);
		}
		if (GAME.Key_Pressing(DIK_RIGHT))
		{
			Vec3 rotation = transform->GetLocalRotation();
			rotation.y += DT * stateMachine->_moveSpeed;
			transform->SetLocalRotation(rotation);
		}
	}
	else if (GAME.Key_Down(DIK_DOWN))
	{
		Vec3 rotation = transform->GetLocalRotation();
		rotation.y -= XM_PI * 0.5f;
		transform->SetLocalRotation(rotation);
	}
	else if (GAME.Key_Pressing(DIK_DOWN))
	{
		stateMachine->_moveSpeed += DT * 1.5f;
		if (stateMachine->_moveSpeed >= 5.f)
			stateMachine->_moveSpeed = 5.f;

		pos += look * DT * stateMachine->_moveSpeed;

		if (GAME.Key_Pressing(DIK_LEFT))
		{
			Vec3 rotation = transform->GetLocalRotation();
			rotation.y -= DT * stateMachine->_moveSpeed;
			transform->SetLocalRotation(rotation);
		}
		if (GAME.Key_Pressing(DIK_RIGHT))
		{
			Vec3 rotation = transform->GetLocalRotation();
			rotation.y += DT * stateMachine->_moveSpeed;
			transform->SetLocalRotation(rotation);
		}
	}
	else if (GAME.Key_Down(DIK_LEFT))
	{
		stateMachine->_moveSpeed = 0.f;

		Vec3 rotation = transform->GetLocalRotation();
		rotation.y -= XM_PI * 0.25f;
		transform->SetLocalRotation(rotation);
	}
	else if (GAME.Key_Down(DIK_RIGHT))
	{
		stateMachine->_moveSpeed = 0.f;

		Vec3 rotation = transform->GetLocalRotation();
		rotation.y += XM_PI * 0.25f;
		transform->SetLocalRotation(rotation);
	}
	else
	{
		stateMachine->_moveSpeed = 0.f;
	}
	transform->SetPosition(pos);



	// 공격
	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		_stateMachine.lock()->ChangeState(L"CorinNormalAttack");
	}
}

void CorinMove::Awake()
{
}

void CorinMove::Start()
{
}

void CorinMove::Update()
{
	Input();
}

void CorinMove::LateUpdate()
{

}

void CorinMove::FixedUpdate()
{
}

HRESULT CorinMove::Render()
{
	return S_OK;
}

void CorinMove::OnCollisionEnter(const Collision& collision)
{
}

void CorinMove::OnCollisionExit(const Collision& collision)
{
}

void CorinMove::OnExit()
{
	auto stateMachine = static_pointer_cast<CorinStateMachineScript>(_stateMachine.lock());
	stateMachine->_moveSpeed = 0.f;
}
