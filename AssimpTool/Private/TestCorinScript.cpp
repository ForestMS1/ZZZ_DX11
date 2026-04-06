#include "pch.h"
#include "TestCorinScript.h"
void TestCorinScript::Start()
{
	shared_ptr<GameObject> pChild = GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"TestSphere");
	if (pChild)
	{
		GetTransform()->AddChild(pChild->GetTransform());
	}

	shared_ptr<Shader> shader = make_shared<Shader>(L"TweenTest.fx");

	shared_ptr<Model> model = make_shared<Model>();
	model->ReadModelRotatedY180(L"Ellen/Ellen");
	model->ReadMaterial(L"Ellen/Ellen");

	model->ReadAnimationRotatedY180NoMove(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk_Start");
	model->ReadAnimationRotatedY180NoMove(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk_Start_End");
	model->ReadAnimationRotatedY180NoMove(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk");
	model->ReadAnimationRotatedY180NoMove(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk_End");
	model->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchIn_Attack_Ex_Start");
	model->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchIn_Normal");
	model->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchOut_Normal");

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

	GetTransform()->SetPosition(pos);
}
