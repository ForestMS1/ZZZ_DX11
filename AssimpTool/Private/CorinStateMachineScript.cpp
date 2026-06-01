#include "pch.h"
#include "CorinStateMachineScript.h"
#include "BaseState.h"

//-----------------State----------------
#include "CorinQuestStart.h"
#include "CorinIdle.h"
#include "CorinMove.h"
#include "CorinEvade.h"
#include "CorinNormalAttack.h"
#include "CorinDashAttack.h"
#include "CorinSwitchOut.h"
#include "CorinSwitchIn.h"
#include "CorinParry.h"
#include "CorinSleep.h"
//--------------------------------------
#include "NetworkView.h"

#include "ModelAnimator.h"

CorinStateMachineScript::CorinStateMachineScript()
{
}

CorinStateMachineScript::CorinStateMachineScript(const CorinStateMachineScript& rhs)
	: StateMachine(rhs)
{
}

CorinStateMachineScript::~CorinStateMachineScript()
{
}

HRESULT CorinStateMachineScript::Initialize(void* pArg)
{

	return S_OK;
}

void CorinStateMachineScript::Awake()
{
	StateMachine::Awake();

	// 상태들 추가~
	shared_ptr<CorinIdle> corinIdle = make_shared<CorinIdle>(GetGameObject(), SHARED_THIS(CorinStateMachineScript));
	shared_ptr<CorinQuestStart> corinQuestStart = make_shared<CorinQuestStart>(GetGameObject(), SHARED_THIS(CorinStateMachineScript));
	shared_ptr<CorinMove> corinMove = make_shared<CorinMove>(GetGameObject(), SHARED_THIS(CorinStateMachineScript));
	shared_ptr<CorinEvade> corinEvade = make_shared<CorinEvade>(GetGameObject(), SHARED_THIS(CorinStateMachineScript));
	shared_ptr<CorinNormalAttack> corinNormalAttack = make_shared<CorinNormalAttack>(GetGameObject(), SHARED_THIS(CorinStateMachineScript));
	shared_ptr<CorinDashAttack> corinDashAttack = make_shared<CorinDashAttack>(GetGameObject(), SHARED_THIS(CorinStateMachineScript));
	shared_ptr<CorinParry> corinParry = make_shared<CorinParry>(GetGameObject(), SHARED_THIS(CorinStateMachineScript));


	AddState(L"CorinIdle", corinIdle);
	AddState(L"CorinQuestStart", corinQuestStart);
	AddState(L"CorinMove", corinMove);
	AddState(L"CorinEvade", corinEvade);
	AddState(L"CorinNormalAttack", corinNormalAttack);
	AddState(L"CorinDashAttack", corinDashAttack);
	AddState(L"CorinParry", corinParry);


	shared_ptr<CorinSwitchOut> corinSwitchOut = make_shared<CorinSwitchOut>(GetGameObject(), SHARED_THIS(CorinStateMachineScript));
	shared_ptr<CorinSwitchIn> corinSwitchIn = make_shared<CorinSwitchIn>(GetGameObject(), SHARED_THIS(CorinStateMachineScript));
	shared_ptr<CorinSleep> corinSleepState = make_shared<CorinSleep>(GetGameObject(), SHARED_THIS(CorinStateMachineScript));
	
	AddState(L"SwitchOut", corinSwitchOut);
	AddState(L"SwitchIn", corinSwitchIn);
	AddState(L"Sleep", corinSleepState);


	// 이벤트 함수 등록
	GAME.Subscribe(static_cast<uint32>(EventType::LEVEL_START), [this](const EventDesc& desc) { this->OnQuestStart(); });


	_curState->Awake();
}

void CorinStateMachineScript::Start()
{
	_curState->Start();
}

void CorinStateMachineScript::Update()
{
	if (auto view = GetGameObject()->GetScript<NetworkView>())
	{
		if (view->IsMine() == false)
			return;
	}


	if (_curStateName == "Sleep")
		return;

	StateMachine::CollectNearMonster(L"Layer_Monster");

	// AnyState 여기서 전역적으로 전이
	if (GAME.Key_Pressing(DIK_UP) || GAME.Key_Pressing(DIK_DOWN))
	{
		_moveSpeed += DT * 10.f;
		if (_moveSpeed >= _moveSpeedMax)
			_moveSpeed = _moveSpeedMax;

		_animator.lock()->SetFloat(L"speed", _moveSpeed);
	}
	else
	{
		_moveSpeed = 0.f;
		_animator.lock()->SetFloat(L"speed", _moveSpeed);
	}

	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_RB))
	{
		ChangeState(L"CorinEvade");
	}

	_curState->Update();

}

void CorinStateMachineScript::LateUpdate()
{
	if (auto view = GetGameObject()->GetScript<NetworkView>())
	{
		if (view->IsMine() == false)
			return;
	}
	_curState->LateUpdate();
}

void CorinStateMachineScript::FixedUpdate()
{
	if (auto view = GetGameObject()->GetScript<NetworkView>())
	{
		if (view->IsMine() == false)
			return;
	}
	_curState->FixedUpdate();


	StateMachine::ClearNearMonsters();
}

HRESULT CorinStateMachineScript::Render()
{

	return _curState->Render();
}

void CorinStateMachineScript::OnCollisionEnter(const Collision& collision)
{
	_curState->OnCollisionEnter(collision);

}

void CorinStateMachineScript::OnCollisionExit(const Collision& collision)
{
	_curState->OnCollisionExit(collision);

}

void CorinStateMachineScript::OnDestroy()
{


}

unique_ptr<CorinStateMachineScript> CorinStateMachineScript::Create()
{
	auto pInstance = unique_ptr<CorinStateMachineScript>(new CorinStateMachineScript);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CorinStateMachineScript");
		return nullptr;
	}

	return pInstance;
}

void CorinStateMachineScript::OnInspectorGUI()
{
	GuiRemoveButton("CorinStateMachineScript");
	if (ImGui::CollapsingHeader("CorinStateMachineScript", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// --- State 정보 ---
		string curStateName = _curState ? _curStateName : "None";
		ImGui::Text("curState : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), _curStateName.c_str());
	}
}

void CorinStateMachineScript::OnQuestStart()
{
	ChangeState(L"CorinQuestStart");
}