#include "pch.h"
#include "TagManagerScript.h"
#include "CorinStateMachineScript.h"
#include "EllenStateMachineScript.h"
void TagManagerScript::Awake()
{
	_playerCharacters.push_back(GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"Corin")->GetScript<CorinStateMachineScript>());
	_playerCharacters.push_back(GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"Ellen")->GetScript<EllenStateMachineScript>());
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


	_playerCharacters[_curActivePlayerIndex]->ChangeState(L"SwitchIn");
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