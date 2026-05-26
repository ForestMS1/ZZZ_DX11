#include "pch.h"
#include "TagManagerScript.h"
#include "CorinStateMachineScript.h"
#include "EllenStateMachineScript.h"
#include "Texture.h"
void TagManagerScript::Awake()
{
	// 씬에 있는 플레이어블 캐릭터 캐싱
	_playerCharacters.push_back(GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"Corin")->GetScript<CorinStateMachineScript>());
	_playerCharacters.push_back(GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"Ellen")->GetScript<EllenStateMachineScript>());


	// 플레이어 수에 맞게 좌상단 UI 텍스쳐 변경
	_playerHpUIBg = GAME.Find_GameObject_fromLayer(L"Layer_UI", L"HPUIBG");
	vector<shared_ptr<Texture>>& textures = _playerHpUIBg->GetSpriteRenderer()->GetTextures();
	textures.clear();

	int playerCnt = _playerCharacters.size();
	switch (playerCnt)
	{
	case 2:
		textures.push_back(GAME.GetResource<Texture>(L"HpBarBg2"));
		break;
	case 3:
		textures.push_back(GAME.GetResource<Texture>(L"HpBarBg3"));
		break;
	default:
		break;
	}

	// 이벤트 함수 등록
	GAME.Subscribe(static_cast<uint32>(EventType::LEVEL_START), [this](const EventDesc& desc) { this->OnQuestStart(); });


}
void TagManagerScript::Start()
{
	_playerIconUIs.push_back(GAME.Find_GameObject_fromLayer(L"Layer_UI", L"IconRole01"));
	_playerIconUIs.push_back(GAME.Find_GameObject_fromLayer(L"Layer_UI", L"IconRole02"));
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
	_lastRotation = _playerCharacters[_curActivePlayerIndex]->GetTransform()->GetRotation();

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
	nextCharacterTransform->SetLocalRotation(_lastRotation);


	// UI 전환
	for (int i = 0; i < _playerIconUIs.size(); ++i)
	{
		int index = (_curActivePlayerIndex + i) % _playerIconUIs.size();
		shared_ptr<StateMachine> player = _playerCharacters[index]->GetGameObject()->GetScript<StateMachine>();
		if (dynamic_pointer_cast<CorinStateMachineScript>(player) != nullptr)
			_playerIconUIs[i]->GetSpriteRenderer()->GetTextures()[0] = GAME.GetResource<Texture>(L"IconRoleGeneral09"); // 코린
		else if(dynamic_pointer_cast<EllenStateMachineScript>(player) != nullptr)
			_playerIconUIs[i]->GetSpriteRenderer()->GetTextures()[0] = GAME.GetResource<Texture>(L"IconRoleGeneral21"); // 엘렌
	}


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
