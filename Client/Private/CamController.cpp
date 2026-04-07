#include "pch.h"
#include "CamController.h"
#include "GameInstance.h"
#include "Transform.h"

CamController::CamController()
{
}

CamController::~CamController()
{
}

void CamController::Update()
{
	ImGui::Begin("MainCam Transform");
	Vec3 pos = GetTransform()->GetPosition();
	Vec3 look = GetTransform()->GetLook();
	ImGui::DragFloat3("pos", (float*)&pos, 1.0f, 0.f, 0.f, "%.3f", 0);
	ImGui::DragFloat3("look", (float*)&look, 1.0f, 0.f, 0.f, "%.3f", 0);
	ImGui::End();
}

void CamController::LateUpdate()
{
	if (!GetGameObject()->GetCamera()->IsActive())
		return;


	Vec3 pos = GetTransform()->GetLocalPosition();
	Vec3 look = GetTransform()->GetLook();
	Vec3 right = GetTransform()->GetRight();
	Vec3 up = GetTransform()->GetUp();

	if (GAME.Key_Pressing(DIK_W))
	{
		pos += look * DT * 5.f;
	}
	if (GAME.Key_Pressing(DIK_S))
	{
		pos -= look * DT * 5.f;
	}
	if (GAME.Key_Pressing(DIK_A))
	{
		pos -= right * DT * 5.f;
	}
	if (GAME.Key_Pressing(DIK_D))
	{
		pos += right * DT * 5.f;
	}

	if (GAME.Key_Pressing(DIK_Q))
	{
		pos += up * DT * 5.f;
	}
	if (GAME.Key_Pressing(DIK_E))
	{
		pos -= up * DT * 5.f;
	}

	if (GAME.Key_Pressing(DIK_Z))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= DT;
		GetTransform()->SetLocalRotation(rotation);
	}
	if (GAME.Key_Pressing(DIK_C))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y += DT;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (GAME.Mouse_Pressing(DIM_RB))
	{
		signed long moveX = GAME.Get_DIMouseMove(DIMS_X);
		signed long moveY = GAME.Get_DIMouseMove(DIMS_Y);

		float sensitivy = 0.1f;

		if (abs(moveX) > 0)
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.y += moveX * sensitivy * DT;
			GetTransform()->SetLocalRotation(rotation);
		}
		if (abs(moveY) > 0)
		{
			Vec3 rotation = GetTransform()->GetLocalRotation();
			rotation.x += moveY * sensitivy * DT;
			GetTransform()->SetLocalRotation(rotation);
		}
	}



	GetTransform()->SetLocalPosition(pos);
}
