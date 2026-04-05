#include "pch.h"
#include "TestCorinScript.h"
void TestCorinScript::Start()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"TweenTest.fx");

	shared_ptr<Model> model = make_shared<Model>();
	model->ReadModelRotatedY180(L"Corin/Corin");
	model->ReadMaterial(L"Corin/Corin");

	model->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Walk_Start");
	model->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Walk");
	model->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Run");

	GetGameObject()->AddComponent(make_shared<ModelAnimator>(shader));
	GetGameObject()->GetModelAnimator()->SetModel(model);

	GetTransform()->SetPosition(Vec3(0, 0, 0));
	GetTransform()->SetScale(Vec3(1.f));
}

void TestCorinScript::Update()
{
	GAME.Add_RenderObject(RENDERGROUP::NONBLEND, GetGameObject());

	Vec3 pos = GetTransform()->GetPosition();
	Vec3 look = GetTransform()->GetLook();
	Vec3 right = GetTransform()->GetRight();
	Vec3 up = GetTransform()->GetUp();

	if (GAME.Key_Pressing(DIK_UP))
	{
		pos += look * DT * 5.f;
	}
	if (GAME.Key_Pressing(DIK_DOWN))
	{
		pos -= look * DT * 5.f;
	}
	if (GAME.Key_Pressing(DIK_LEFT))
	{
		pos -= right * DT * 5.f;
	}
	if (GAME.Key_Pressing(DIK_RIGHT))
	{
		pos += right * DT * 5.f;
	}

	GetTransform()->SetPosition(pos);
}
