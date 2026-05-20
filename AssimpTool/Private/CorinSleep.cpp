#include "pch.h"
#include "CorinSleep.h"

CorinSleep::CorinSleep(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinSleep::~CorinSleep()
{
}

void CorinSleep::OnEnter()
{

}

void CorinSleep::Input()
{

}

void CorinSleep::Awake()
{
}

void CorinSleep::Start()
{
}

void CorinSleep::Update()
{

}

void CorinSleep::LateUpdate()
{

}

void CorinSleep::FixedUpdate()
{
}

HRESULT CorinSleep::Render()
{
	return S_OK;
}

void CorinSleep::OnExit()
{
}
