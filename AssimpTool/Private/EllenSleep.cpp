#include "pch.h"
#include "EllenSleep.h"

EllenSleep::EllenSleep(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

EllenSleep::~EllenSleep()
{
}

void EllenSleep::OnEnter()
{

}

void EllenSleep::Input()
{

}

void EllenSleep::Awake()
{
}

void EllenSleep::Start()
{
}

void EllenSleep::Update()
{

}

void EllenSleep::LateUpdate()
{

}

void EllenSleep::FixedUpdate()
{
}

HRESULT EllenSleep::Render()
{
	return S_OK;
}

void EllenSleep::OnExit()
{
}
