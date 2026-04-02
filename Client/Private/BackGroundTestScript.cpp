#include "pch.h"
#include "BackGroundTestScript.h"
#include "GameInstance.h"
#include "Transform.h"
BackGroundTestScript::BackGroundTestScript()
{
}

BackGroundTestScript::~BackGroundTestScript()
{
}

void BackGroundTestScript::Awake()
{
	int a = 1;
}

void BackGroundTestScript::Start()
{

}

void BackGroundTestScript::Update()
{
	GAME.Add_RenderObject(RENDERGROUP::NONBLEND, GetGameObject());
}

void BackGroundTestScript::LateUpdate()
{
	int a = 1;
}

void BackGroundTestScript::FixedUpdate()
{
	int a = 1;
}

shared_ptr<Prototype> BackGroundTestScript::Clone(void* pArg)
{
	return shared_ptr<Prototype>();
}
