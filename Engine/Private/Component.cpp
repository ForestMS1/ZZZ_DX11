#include "pch.h"
#include "Component.h"
#include "GameObject.h"

Component::Component(ComponentType eType) : _type(eType)
{
}

Component::~Component()
{
}

HRESULT Component::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Component::Initialize(void* pArg)
{
	return S_OK;
}

shared_ptr<GameObject> Component::GetGameObject()
{
	return _gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
	if (_gameObject.lock() == nullptr)
		return nullptr;

	return _gameObject.lock()->GetTransform();
}

void Component::GuiRemoveButton(string componentName)
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
	if (ImGui::Button(componentName.append(" Remove").c_str()))
	{
		uint8 index = static_cast<uint8>(_type);
		if (index < FIXED_COMPONENT_COUNT)
		{
			GetGameObject()->RemoveFixedComponent(_type);
		}
		else
		{
			// 템플릿 호출 대신, 현재 인스펙터가 가리키고 있는 '나 자신'만 삭제
			RemoveSelf();
		}
	}
	ImGui::PopStyleColor();
}
