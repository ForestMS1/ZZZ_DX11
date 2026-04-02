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
	GAME.Add_RenderObject(RENDERGROUP::BLEND, GetGameObject());

	ImGui::Begin("BackGround Transform");
	Vec3 pos = GetTransform()->GetPosition();
	Vec3 scale = GetTransform()->GetScale();
	ImGui::DragFloat3("Position", (float*)&pos, 1.0f, -100.f, 100.f, "%.3f", 1);
	ImGui::DragFloat3("Scale", (float*)&scale, 1.0f, -100.f, 100.f, "%.3f", 1);

	GetTransform()->SetPosition(pos);
	GetTransform()->SetScale(scale);
	ImGui::End();
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
