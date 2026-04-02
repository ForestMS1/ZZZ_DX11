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
	int a = 1;
}

void BackGroundTestScript::Update()
{
	GameInstance::Get().Add_RenderObject(RENDERGROUP::UI, GetGameObject());
	Vec3 pos = GetTransform()->GetPosition();

	if (GetKeyState(VK_UP) & 0x8000)
	{
		pos += Vec3(0.f, 1.f, 0.f) * DT;
	}
	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		pos += Vec3(0.f, -1.f, 0.f) * DT;
	}
	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		pos += Vec3(-1.f, 0.f, 0.f) * DT;
	}
	if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		pos += Vec3(1.f, 0.f, 0.f) * DT;
	}

	GetTransform()->SetPosition(pos);
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
