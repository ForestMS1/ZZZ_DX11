#include "pch.h"
#include "Collider.h"

Collider::Collider(ColliderType type)
	: Component(ComponentType::Collider), _colliderType(type)
{
   
}

Collider::~Collider()
{
}

void Collider::OnInspectorGUI()
{
    GuiRemoveButton("Collider");

    ImGui::Checkbox("isTrigger", &_isTrigger);
    ImGui::SameLine();
    ImGui::Checkbox("isFix", &_isFix);

    if (ImGui::TreeNodeEx("Collider", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 현재 콜라이더 타입 표시
        const char* typeStr = "";
        switch (_colliderType)
        {
        case ColliderType::AABB:   typeStr = "AABB (Axis Aligned)"; break;
        case ColliderType::OBB:    typeStr = "OBB (Oriented)"; break;
        case ColliderType::Sphere: typeStr = "Sphere"; break;
        }
        ImGui::Text("Type: %s", typeStr);


        ImGui::TreePop();
    }
}

