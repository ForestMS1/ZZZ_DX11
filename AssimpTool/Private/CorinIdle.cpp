#include "pch.h"
#include "CorinIdle.h"
#include "CorinStateMachineScript.h"

CorinIdle::CorinIdle(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinIdle::~CorinIdle()
{
}

void CorinIdle::OnEnter()
{
	if (GAME.Key_Pressing(DIK_UP) || GAME.Key_Pressing(DIK_DOWN))
		_animator.lock()->SetBool(L"isMove", true);
	else
		_animator.lock()->SetBool(L"isMove", false);
}

void CorinIdle::Input()
{
	if(GAME.Key_Pressing(DIK_UP) || GAME.Key_Pressing(DIK_DOWN))
		_animator.lock()->SetBool(L"isMove", true);
	else
		_animator.lock()->SetBool(L"isMove", false);


	auto transform = _gameObject.lock()->GetTransform();

	Vec3 pos = transform->GetPosition();
	Vec3 look = transform->GetLook();
	Vec3 right = transform->GetRight();
	Vec3 up = transform->GetUp();

	auto stateMachine = static_pointer_cast<CorinStateMachineScript>(_stateMachine.lock());

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
		_animator.lock()->SetBool(L"attackNormal", true);
		_animator.lock()->SetFloat(L"attackNormalStep", 0.f);
		_stateMachine.lock()->ChangeState(L"CorinNormalAttack");
	}


	// Evade
	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_RB))
	{
		_stateMachine.lock()->ChangeState(L"CorinEvade");
	}
}

void CorinIdle::Awake()
{
}

void CorinIdle::Start()
{
}

void CorinIdle::Update()
{
	Input();
}

void CorinIdle::LateUpdate()
{
}

void CorinIdle::FixedUpdate()
{
}

HRESULT CorinIdle::Render()
{
	return S_OK;
}

void CorinIdle::OnCollisionEnter(const Collision& collision)
{
}

void CorinIdle::OnCollisionExit(const Collision& collision)
{
}

void CorinIdle::OnExit()
{
	//auto animFSM = _animStateMachine.lock();
	//animFSM->SetBool(L"isMove", false);
}
