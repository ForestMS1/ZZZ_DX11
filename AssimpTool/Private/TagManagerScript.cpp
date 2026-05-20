#include "pch.h"
#include "TagManagerScript.h"
#include "CorinStateMachineScript.h"
#include "EllenStateMachineScript.h"
void TagManagerScript::Awake()
{
	_playerCharacters.push_back(GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"Corin")->GetScript<CorinStateMachineScript>());
	_playerCharacters.push_back(GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"Ellen")->GetScript<EllenStateMachineScript>());

	// 이벤트 함수 등록
	GAME.Subscribe(static_cast<uint32>(EventType::LEVEL_START), [this](const EventDesc& desc) { this->OnQuestStart(); });
}
void TagManagerScript::Start()
{

}

void TagManagerScript::Update()
{
	if (GAME.Key_Down(DIK_SPACE))
	{
		ChangeCharacter();
	}

}


void TagManagerScript::OnDestroy()
{

}


unique_ptr<TagManagerScript> TagManagerScript::Create()
{
	auto pInstance = unique_ptr<TagManagerScript>(new TagManagerScript);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TagManagerScript");
		return nullptr;
	}

	return pInstance;
}

void TagManagerScript::ChangeCharacter()
{
	// 나와있는 캐릭터의 마지막 위치 저장
	_lastPosition = _playerCharacters[_curActivePlayerIndex]->GetTransform()->GetPosition();

	_playerCharacters[_curActivePlayerIndex]->ChangeState(L"SwitchOut");

	_curActivePlayerIndex = (_curActivePlayerIndex + 1) % _playerCharacters.size();

	// 이벤트 발행
	auto desc = make_shared<EventDesc>();
	desc->sender = this;
	desc->eventType = static_cast<uint32>(EventType::CHARACTER_SWITCH);
	GAME.PushEvent(desc);

	auto nextCharacterTransform = _playerCharacters[_curActivePlayerIndex]->GetTransform();
	Vec3 nextPosition;
	nextPosition = _lastPosition + nextCharacterTransform->GetRight() * 2.f;
	nextCharacterTransform->SetLocalPosition(nextPosition);


	_playerCharacters[_curActivePlayerIndex]->ChangeState(L"SwitchIn");
}

void TagManagerScript::OnQuestStart()
{
	for (uint8 i = 0; i < _playerCharacters.size(); ++i)
	{
		if (i == _curActivePlayerIndex)
			continue;
		_playerCharacters[i]->ChangeState(L"Sleep");
	}
}


void TagManagerScript::OnInspectorGUI()
{
	GuiRemoveButton("TagManagerScript");
	if (ImGui::CollapsingHeader("TagManagerScript", ImGuiTreeNodeFlags_DefaultOpen))
	{
		string playerCharacterCount = to_string(_playerCharacters.size());
		string curPlayereIndex = to_string(_curActivePlayerIndex);
		ImGui::Text("PlayerCount : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), playerCharacterCount.c_str());
		ImGui::Text("curPlayereIndex : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), curPlayereIndex.c_str());
	}
}

shared_ptr<GameObject> TagManagerScript::GetCurCharacter()
{
	return _playerCharacters[_curActivePlayerIndex]->GetGameObject();
}
