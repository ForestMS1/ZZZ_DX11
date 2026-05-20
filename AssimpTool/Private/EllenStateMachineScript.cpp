#include "pch.h"
#include "EllenStateMachineScript.h"
#include "BaseState.h"

//-----------------State----------------
#include "EllenIdle.h"
#include "EllenEvade.h"
#include "EllenNormalAttack.h"
#include "EllenSwitchOut.h"
#include "EllenSwitchIn.h"
//--------------------------------------
#include "NetworkView.h"

#include "ModelAnimator.h"

EllenStateMachineScript::EllenStateMachineScript()
{
}

EllenStateMachineScript::EllenStateMachineScript(const EllenStateMachineScript& rhs)
	: StateMachine(rhs)
{
}

EllenStateMachineScript::~EllenStateMachineScript()
{
}

HRESULT EllenStateMachineScript::Initialize(void* pArg)
{

	return S_OK;
}

void EllenStateMachineScript::Awake()
{
	StateMachine::Awake();

	// 상태들 추가~
	shared_ptr<EllenIdle> ellenIdle = make_shared<EllenIdle>(GetGameObject(), SHARED_THIS(EllenStateMachineScript));
	shared_ptr<EllenEvade> ellenEvade = make_shared<EllenEvade>(GetGameObject(), SHARED_THIS(EllenStateMachineScript));
	shared_ptr<EllenNormalAttack> ellenNormalAttack = make_shared<EllenNormalAttack>(GetGameObject(), SHARED_THIS(EllenStateMachineScript));

	AddState(L"EllenIdle", ellenIdle);
	AddState(L"EllenEvade", ellenEvade);
	AddState(L"EllenNormalAttack", ellenNormalAttack);

	shared_ptr<EllenSwitchOut> ellenSwitchOut = make_shared<EllenSwitchOut>(GetGameObject(), SHARED_THIS(EllenStateMachineScript));
	shared_ptr<EllenSwitchIn> ellenSwitchIn = make_shared<EllenSwitchIn>(GetGameObject(), SHARED_THIS(EllenStateMachineScript));
	AddState(L"SwitchOut", ellenSwitchOut);
	AddState(L"SwitchIn", ellenSwitchIn);
	//AddState(L"EllenSleep", ellenSleep);


	// 이벤트 함수 등록
	GAME.Subscribe(static_cast<uint32>(EventType::LEVEL_START), [this](const EventDesc& desc) { this->OnQuestStart(); });


	_curState->Awake();
}

void EllenStateMachineScript::Start()
{
	_curState->Start();
}

void EllenStateMachineScript::Update()
{
	if (auto view = GetGameObject()->GetScript<NetworkView>())
	{
		if (view->IsMine() == false)
			return;
	}

	// AnyState 여기서 전역적으로 전이

	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_RB))
	{
		ChangeState(L"EllenEvade");
	}

	_curState->Update();

}

void EllenStateMachineScript::LateUpdate()
{
	if (auto view = GetGameObject()->GetScript<NetworkView>())
	{
		if (view->IsMine() == false)
			return;
	}
	_curState->LateUpdate();
}

void EllenStateMachineScript::FixedUpdate()
{
	if (auto view = GetGameObject()->GetScript<NetworkView>())
	{
		if (view->IsMine() == false)
			return;
	}
	_curState->FixedUpdate();

}

HRESULT EllenStateMachineScript::Render()
{

	return _curState->Render();
}

void EllenStateMachineScript::OnCollisionEnter(const Collision& collision)
{
	_curState->OnCollisionEnter(collision);

}

void EllenStateMachineScript::OnCollisionExit(const Collision& collision)
{
	_curState->OnCollisionExit(collision);

}

void EllenStateMachineScript::OnDestroy()
{


}

unique_ptr<EllenStateMachineScript> EllenStateMachineScript::Create()
{
	auto pInstance = unique_ptr<EllenStateMachineScript>(new EllenStateMachineScript);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : EllenStateMachineScript");
		return nullptr;
	}

	return pInstance;
}

void EllenStateMachineScript::OnInspectorGUI()
{
	GuiRemoveButton("EllenStateMachineScript");
	if (ImGui::CollapsingHeader("EllenStateMachineScript", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// --- State 정보 ---
		string curStateName = _curState ? _curStateName : "None";
		ImGui::Text("curState : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), _curStateName.c_str());
	}
}

void EllenStateMachineScript::OnQuestStart()
{
	_animator.lock()->SetTrigger(L"questStart");
}