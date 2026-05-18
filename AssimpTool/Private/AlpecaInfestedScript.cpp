#include "pch.h"
#include "AlpecaInfestedScript.h"


#include "AnimFSM.h"
#include "Transition.h"
#include "AnimState.h"
#include "NetworkView.h"
void AlpecaInfestedScript::Awake()
{

}
void AlpecaInfestedScript::Start()
{

}

void AlpecaInfestedScript::Update()
{
	
}

void AlpecaInfestedScript::OnCollisionEnter(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	collider->SetDebugColor(Colors::Red);
}

void AlpecaInfestedScript::OnCollisionStay(const Collision& collision)
{
	_curHitDelay += DT;
	if (collision.colliderA.lock()->GetGameObject()->GetName() == L"CorinWeapon" || collision.colliderB.lock()->GetGameObject()->GetName() == L"CorinWeapon")
	{
		if (auto animator = GetGameObject()->GetModelAnimator())
		{
			if (_curHitDelay >= _onHitDelay)
			{
				animator->SetTrigger(L"onHit");
				_curHitDelay = 0.f;
			}
		}
	}
}

void AlpecaInfestedScript::OnCollisionExit(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	collider->SetDebugColor(Colors::LimeGreen);

	_curHitDelay = 0.f;
}

void AlpecaInfestedScript::OnDestroy()
{
}


unique_ptr<AlpecaInfestedScript> AlpecaInfestedScript::Create()
{
	auto pInstance = unique_ptr<AlpecaInfestedScript>(new AlpecaInfestedScript);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : AlpecaInfestedScript");
		return nullptr;
	}

	return pInstance;
}


void AlpecaInfestedScript::OnInspectorGUI()
{
	GuiRemoveButton("AlpecaInfestedScript");
	if (ImGui::CollapsingHeader("AlpecaInfestedScript", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}