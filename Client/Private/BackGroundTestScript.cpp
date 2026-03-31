#include "pch.h"
#include "BackGroundTestScript.h"

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
	int a = 1;
}

void BackGroundTestScript::Update()
{
	GameInstance::Get().Add_RenderObject(RENDERGROUP::UI, GetGameObject());
	int a = 1;
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
