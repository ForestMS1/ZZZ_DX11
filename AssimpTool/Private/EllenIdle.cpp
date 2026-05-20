#include "pch.h"
#include "EllenIdle.h"
#include "EllenStateMachineScript.h"

EllenIdle::EllenIdle(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

EllenIdle::~EllenIdle()
{
}

void EllenIdle::OnEnter()
{
	auto stateMachine = static_pointer_cast<EllenStateMachineScript>(_stateMachine.lock());
	_animator.lock()->SetFloat(L"speed", stateMachine->_moveSpeed);
}

void EllenIdle::Input()
{
	auto transform = _gameObject.lock()->GetTransform();
	auto stateMachine = static_pointer_cast<EllenStateMachineScript>(_stateMachine.lock());


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



	// АјАн
	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		_stateMachine.lock()->ChangeState(L"EllenNormalAttack");
	}
}

void EllenIdle::Awake()
{
}

void EllenIdle::Start()
{
}

void EllenIdle::Update()
{
	Input();
}

void EllenIdle::LateUpdate()
{

}

void EllenIdle::FixedUpdate()
{
}

HRESULT EllenIdle::Render()
{
	return S_OK;
}

void EllenIdle::OnCollisionEnter(const Collision& collision)
{
}

void EllenIdle::OnCollisionExit(const Collision& collision)
{
}

void EllenIdle::OnExit()
{

}
