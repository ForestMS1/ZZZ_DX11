#include "pch.h"
#include "TestCorinScript.h"
#include "AnimFSM.h"
#include "Transition.h"
#include "AnimState.h"
void TestCorinScript::Awake()
{

}
void TestCorinScript::Start()
{

}

void TestCorinScript::Update()
{
	shared_ptr<AnimFSM> fsm = GetGameObject()->GetModelAnimator()->GetFSM();
	fsm->SetBool(L"isMove", true);

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
	else
	{
		fsm->SetBool(L"isMove", false);
	}

	if (fsm->GetCurAnimState()->GetName().compare(L"Idle"))
		GetTransform()->SetPosition(pos);
}
