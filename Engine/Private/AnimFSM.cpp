#include "pch.h"
#include "AnimFSM.h"
#include "ModelAnimator.h"
#include "AnimState.h"
#include "Transition.h"
#include "Model.h"
#include "imnodes.h"
#include "ModelAnimation.h"
#include "Model.h"
#include <filesystem>
#include "FileUtils.h"
#include "Condition.h"

AnimFSM::AnimFSM(shared_ptr<ModelAnimator> animatorComponent)
    : _animatorComponent(animatorComponent)
{
}

AnimFSM::~AnimFSM()
{
}


void AnimFSM::Update()
{
    if (_curAnimState == nullptr) 
        return;

    // 현재 상태의 트랜지션들을 검사
    for (auto& transition : _curAnimState->GetTransitions())
    {
        if (transition->CanTransition())
        {
            ApplyTransition(transition);
            break;
        }
        else
        {
            shared_ptr<ModelAnimator> animator = _animatorComponent.lock();

            TweenDesc& desc = animator->GetTweenDesc();
            desc.curr.animIndex = _curAnimState->GetAnimationClipIndex();
        }
    }

    /*{
        shared_ptr<ModelAnimator> animator = _animatorComponent.lock();
        shared_ptr<Model> model = animator->GetModel();
        shared_ptr<ModelAnimation> clip = _curAnimState->GetAnimationClip();

        TweenDesc& desc = animator->GetTweenDesc();
        desc.curr.animIndex = model->GetAnimationIndexByName(clip->name);
    }*/
}

void AnimFSM::ApplyTransition(shared_ptr<Transition> transition)
{
    shared_ptr<AnimState> nextState = transition->GetToState();
    shared_ptr<ModelAnimator> animator = _animatorComponent.lock();
    shared_ptr<Model> model = animator->GetModel();
    if (animator == nullptr || nextState == nullptr || model == nullptr) return;


    TweenDesc& desc = animator->GetTweenDesc();
    if (desc.next.animIndex >= 0)
        return; // 이미 섞는 중이면 새로운 트랜지션 무시 (혹은 즉시 교체 로직)

    _curTransition = transition;

    desc.next.animIndex = nextState->GetAnimationClipIndex();
}

void AnimFSM::ChangeState(const wstring& stateName)
{
	//if (_curAnimState)
	//	_curAnimState->OnExit();

	_curAnimState = _animStates[stateName];
	//_curAnimState->OnEnter();

}

void AnimFSM::Add_AnimState(const wstring& stateKey, shared_ptr<AnimState> state)
{
    auto iter = _animStates.find(stateKey);
    if (iter != _animStates.end())
    {
        return;
    }
    state->SetName(stateKey);
    _animStates.emplace(stateKey, state);
}

//TweenDesc Property
TweenDesc& AnimFSM::GetTweenDesc()
{ 
	return _animatorComponent.lock()->GetTweenDesc();
}

void AnimFSM::SetTweenDesc(const TweenDesc& desc)
{ 
	_animatorComponent.lock()->SetTweenDesc(desc);
}

void AnimFSM::UpdateParamByHash(uint32_t paramHash, float value)
{
    // Float 파라미터에서 찾기
    for (auto& [name, val] : _floatParams)
    {
        if (Utils::Hash(name) == paramHash)
        {
            val = value;
            return; // 찾았으면 종료
        }
    }

    // Bool 파라미터에서 찾기
    for (auto& [name, val] : _boolParams)
    {
        if (Utils::Hash(name) == paramHash)
        {
            val = (value != 0.0f); // 0이 아니면 true
            return;
        }
    }
}

void AnimFSM::OnInspectorGUI()
{
    ImGui::Begin("Animator FSM Editor");

    if (ImGui::Button("Save"))
    {
        Save("CorinFSM");
    }
    // 파라미터 확인용
    if (ImGui::CollapsingHeader("Blackboard", ImGuiTreeNodeFlags_DefaultOpen)) {
        RenderParameters(); 
    }


    ImNodes::BeginNodeEditor();
    ImNodes::PushStyleVar(ImNodesStyleVar_LinkLineSegmentsPerLength, 0.0001f);
    // State 그리기
    for (auto& [name, state] : _animStates)
    {
        int nodeID = (int)state.get();//state->GetID();

        ImNodes::BeginNode(nodeID);

        // 노드 제목
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(Utils::ToString(name).c_str());
        ImNodes::EndNodeTitleBar();

        // 입력 핀
        ImNodes::BeginInputAttribute(nodeID << 1);
        ImGui::Dummy(ImVec2(10, 20)); // 왼쪽 판정 범위
        ImNodes::EndInputAttribute();

        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::Text("State: %s", Utils::ToString(name).c_str());
        ImGui::EndGroup();
        ImGui::SameLine();

        ImNodes::BeginOutputAttribute((nodeID << 1) | 1);
        ImGui::Dummy(ImVec2(10, 20)); // 오른쪽 판정 범위
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();

        if (ImNodes::IsNodeSelected(nodeID))
            DrawStateDetailEditor(state);
    }


    static shared_ptr<Transition> selectedTransition = nullptr;
    // Transition 그리기
    int linkID = 0;
    for (auto& [name, state] : _animStates)
    {
        int fromNodeID = (int)state.get(); //state->GetID();
        for (auto& trans : state->GetTransitions())
        {
            auto toState = trans->GetToState();
            if (!toState) continue;

            // Link ID, FromPin ID, ToPin ID
            ImNodes::Link(linkID++, (fromNodeID << 1) | 1, (/*toState->GetID()*/(int)toState.get() << 1));
        }
    }

    ImNodes::EndNodeEditor();


    // --- 링크(Transition) 선택 감지 로직 ---
    int selectedLinkID = -1;
    if (ImNodes::NumSelectedLinks() > 0)
    {
        // 가장 최근에 선택된 링크 ID 하나를 가져옴
        int selectedLinkIDs[1];
        ImNodes::GetSelectedLinks(selectedLinkIDs);
        selectedLinkID = selectedLinkIDs[0];
    }

    // 링크 ID를 기반으로 어떤 트랜지션인지 찾아냄
    selectedTransition = nullptr; // 매 프레임 초기화 후 다시 찾기
    int currentLinkCheckID = 0;
    for (auto& [name, state] : _animStates)
    {
        for (auto& trans : state->GetTransitions())
        {
            if (currentLinkCheckID == selectedLinkID)
            {
                selectedTransition = trans;
                break;
            }
            currentLinkCheckID++;
        }
        if (selectedTransition) break;
    }

    // 상세 정보창 띄우기
    if (selectedTransition)
    {
        DrawTransitionDetailEditor(selectedTransition);
    }

    HandleLinkCreation();


    bool openNameInput = false; // 플래그 변수

    // --- 우클릭 메뉴 처리 ---
    // 노드 에디터 빈 공간 우클릭 시
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup("NodeEditorContextMenu");
    }

    if (ImGui::BeginPopup("NodeEditorContextMenu"))
    {
        if (ImGui::MenuItem("Add New State"))
        {
            openNameInput = true;
        }

        if (ImGui::MenuItem("Delete Selected", "Del", false, ImNodes::NumSelectedNodes() > 0))
        {
            int num_selected = ImNodes::NumSelectedNodes();
            std::vector<int> selected_nodes(num_selected);
            ImNodes::GetSelectedNodes(selected_nodes.data());

            for (int nodeID : selected_nodes)
                RemoveState(nodeID);

            ImNodes::ClearNodeSelection();
        }
        ImGui::EndPopup();
    }

    if (openNameInput)
    {
        ImGui::OpenPopup("CreateStateNamePopup");
    }

    // 이름을 입력받는 중간 팝업
    if (ImGui::BeginPopup("CreateStateNamePopup"))
    {
        static char newNameBuffer[128] = ""; // 상태 이름 입력 버퍼

        ImGui::Text("Enter State Name:");
        // 텍스트 필드에 포커스 자동 부여
        if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();

        bool res = ImGui::InputText("##NewStateName", newNameBuffer, IM_ARRAYSIZE(newNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

        if (res || ImGui::Button("Create", ImVec2(120, 0)))
        {
            wstring newName = Utils::ToWString(newNameBuffer);

            // 중복 체크 및 빈 이름 방지
            if (!newName.empty() && _animStates.find(newName) == _animStates.end())
            {
                auto newState = make_shared<AnimState>(shared_from_this());
                newState->SetName(newName);

                // 맵에 추가
                Add_AnimState(newName, newState);

                // 팝업이 열렸던 마우스 위치에 노드 배치 (이전 프레임의 마우스 위치 활용)
                ImNodes::SetNodeScreenSpacePos((int)newState.get(), ImGui::GetMousePos());

                // 버퍼 초기화 및 팝업 닫기
                newNameBuffer[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            newNameBuffer[0] = '\0';
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // 노드 드래그 시 위치 저장
    // ImNodes::IsNodeSelected() 등을 이용해 상세 정보를 DrawStateDetailEditor()로 

    ImGui::End();
}

void AnimFSM::HandleLinkCreation()
{
    int start_pin, end_pin;
    // 새로운 링크가 연결되었는지 체크
    if (ImNodes::IsLinkCreated(&start_pin, &end_pin))
    {
        // 핀 ID 규칙: nodeID << 1 (In), (nodeID << 1) | 1 (Out)
        // 거꾸로 계산해서 노드 주소(ID)를 알아냄
        AnimState* fromStatePtr = (AnimState*)(start_pin >> 1);
        AnimState* toStatePtr = (AnimState*)(end_pin >> 1);

        // 만약 거꾸로 연결했다면 (In -> Out) 스왑 처리
        if (start_pin % 2 == 0) std::swap(fromStatePtr, toStatePtr);

        if (fromStatePtr && toStatePtr)
        {
            // 실제 데이터 구조에 Transition 추가
            auto newTrans = make_shared<Transition>(shared_from_this());

            // 포인터로부터 shared_ptr를 안전하게 가져오는 로직 필요
            // (예: _animStates 맵에서 검색하거나 State가 shared_from_this() 지원)
            auto toStateShared = _animStates[toStatePtr->GetName()];
            newTrans->SetToState(toStateShared);

            fromStatePtr->Add_Transition(newTrans);
        }
    }
}

void AnimFSM::Save(const string& fileName)
{
    // 경로 설정 (파일명 포함)
    string fullPath = "../../Saved/FSM/" + fileName + ".xml";

    _fileName = Utils::ToWString(fileName);

    // 폴더 생성
    filesystem::create_directories("../../Saved/FSM/");

    shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();
    tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
    document->LinkEndChild(decl);

    tinyxml2::XMLElement* root = document->NewElement("AnimFSM");
    document->LinkEndChild(root);

    // Parameters 저장 (Blackboard)
    tinyxml2::XMLElement* paramNode = document->NewElement("Parameters");
    root->LinkEndChild(paramNode);

    for (auto& [name, val] : _boolParams)
    {
        tinyxml2::XMLElement* e = document->NewElement("Bool");
        e->SetAttribute("Name", Utils::ToString(name).c_str());
        e->SetAttribute("Value", val);
        paramNode->LinkEndChild(e);
    }
    for (auto& [name, val] : _floatParams)
    {
        tinyxml2::XMLElement* e = document->NewElement("Float");
        e->SetAttribute("Name", Utils::ToString(name).c_str());
        e->SetAttribute("Value", val);
        paramNode->LinkEndChild(e);
    }

    // AnimStates 저장
    for (auto& [name, state] : _animStates)
    {
        tinyxml2::XMLElement* stateNode = document->NewElement("AnimState");
        root->LinkEndChild(stateNode);

        stateNode->SetAttribute("Name", Utils::ToString(state->GetName()).c_str());
        stateNode->SetAttribute("ClipName", Utils::ToString(state->GetAnimationClip()->name).c_str());

        // ImNodes 위치 저장 (추가한다면)
        // stateNode->SetAttribute("PosX", state->GetEditorPos().x);
        // stateNode->SetAttribute("PosY", state->GetEditorPos().y);

        // Transitions 저장
        for (auto& transition : state->GetTransitions())
        {
            tinyxml2::XMLElement* transNode = document->NewElement("Transition");
            stateNode->LinkEndChild(transNode);

            transNode->SetAttribute("ToState", Utils::ToString(transition->GetToState()->GetName()).c_str());
            transNode->SetAttribute("Duration", transition->GetTransitionDuration());
            transNode->SetAttribute("HasExitTime", transition->HasExitTime());

            // Transition::Save 로직의 일부라고 가정
            for (auto& condition : transition->GetConditions())
            {
                tinyxml2::XMLElement* condNode = document->NewElement("Condition");
                transNode->LinkEndChild(condNode);

                // 공통 속성: 파라미터 이름과 비교 모드
                condNode->SetAttribute("ParamName", Utils::ToString(condition->_paramName).c_str());
                condNode->SetAttribute("Mode", static_cast<int>(condition->_mode));

                // 타입별 데이터 저장
                if (auto floatCond = dynamic_pointer_cast<FloatCondition>(condition))
                {
                    condNode->SetAttribute("Type", "Float");
                    condNode->SetAttribute("Value", floatCond->_value);
                }
                else if (auto boolCond = dynamic_pointer_cast<BoolCondition>(condition))
                {
                    condNode->SetAttribute("Type", "Bool");
                    condNode->SetAttribute("Value", boolCond->_value);
                }
            }
        }
    }

    document->SaveFile(fullPath.c_str());
}

void AnimFSM::Load(const string& fileName, shared_ptr<ModelAnimator> animatorComponent)
{
    _animatorComponent = animatorComponent;
    string fullPath = "../../Saved/FSM/" + fileName + ".xml";

    _fileName = Utils::ToWString(fileName);

    tinyxml2::XMLDocument document;
    if (document.LoadFile(fullPath.c_str()) != tinyxml2::XML_SUCCESS)
        return;

    tinyxml2::XMLElement* root = document.FirstChildElement("AnimFSM");
    if (!root) return;

    // 기존 데이터 초기화
    _animStates.clear();
    _boolParams.clear();
    _floatParams.clear();

    // Parameters 로드
    tinyxml2::XMLElement* paramNode = root->FirstChildElement("Parameters");
    if (paramNode)
    {
        for (auto* e = paramNode->FirstChildElement("Bool"); e; e = e->NextSiblingElement("Bool"))
            AddBool(Utils::ToWString(e->Attribute("Name")), e->BoolAttribute("Value"));

        for (auto* e = paramNode->FirstChildElement("Float"); e; e = e->NextSiblingElement("Float"))
            AddFloat(Utils::ToWString(e->Attribute("Name")), e->FloatAttribute("Value"));
    }

    // [Pass 1] 모든 AnimState 생성 (Transition 제외)
    for (auto* stateNode = root->FirstChildElement("AnimState"); stateNode; stateNode = stateNode->NextSiblingElement("AnimState"))
    {
        wstring stateName = Utils::ToWString(stateNode->Attribute("Name"));
        string clipName = stateNode->Attribute("ClipName");

        auto newState = make_shared<AnimState>(shared_from_this());
        newState->SetName(stateName);

        // 모델에서 애니메이션 클립 찾아 연결
        if (clipName != "None" && animatorComponent->GetModel())
        {
            auto clip = animatorComponent->GetModel()->GetAnimationByName(Utils::ToWString(clipName));
            newState->SetAnimationClip(clip);
        }

        Add_AnimState(stateName, newState);
    }

    // [Pass 2] Transition 및 Condition 연결
    for (auto* stateNode = root->FirstChildElement("AnimState"); stateNode; stateNode = stateNode->NextSiblingElement("AnimState"))
    {
        wstring fromName = Utils::ToWString(stateNode->Attribute("Name"));
        auto fromState = _animStates[fromName];

        for (auto* transNode = stateNode->FirstChildElement("Transition"); transNode; transNode = transNode->NextSiblingElement("Transition"))
        {
            auto transition = make_shared<Transition>(shared_from_this());

            // 목적지 상태 연결
            wstring toName = Utils::ToWString(transNode->Attribute("ToState"));
            transition->SetToState(_animStates[toName]);
            transition->SetTransitionDuration(transNode->FloatAttribute("Duration"));
            transition->SetHasExitTime(transNode->BoolAttribute("HasExitTime"));

            // Condition 로드
            for (auto* condNode = transNode->FirstChildElement("Condition"); condNode; condNode = condNode->NextSiblingElement("Condition"))
            {
                string type = condNode->Attribute("Type");
                shared_ptr<Condition> condition = nullptr;

                if (type == "Float")
                {
                    auto floatCond = make_shared<FloatCondition>();
                    floatCond->_value = condNode->FloatAttribute("Value");
                    condition = floatCond;
                }
                else if (type == "Bool")
                {
                    auto boolCond = make_shared<BoolCondition>();
                    boolCond->_value = condNode->BoolAttribute("Value");
                    condition = boolCond;
                }

                if (condition)
                {
                    condition->_paramName = Utils::ToWString(condNode->Attribute("ParamName"));
                    condition->_mode = static_cast<ConditionMode>(condNode->IntAttribute("Mode"));
                    transition->Add_Condition(condition);
                }
            }
            fromState->Add_Transition(transition);
        }
    }

    // 초기 상태 설정
    if (!_animStates.empty())
    {
        if (_animStates.count(L"Idle")) _curAnimState = _animStates[L"Idle"];
        else _curAnimState = _animStates.begin()->second;
    }
}

void AnimFSM::RenderParameters()
{
    // --- Bool Parameters ---
    if (ImGui::TreeNode("Booleans"))
    {
        for (auto it = _boolParams.begin(); it != _boolParams.end(); )
        {
            string label = Utils::ToString(it->first);
            ImGui::PushID(label.c_str());

            if (ImGui::Button("X")) {
                RemoveParameter(it->first, false);
                it = _boolParams.begin(); // 맵 변형 시 안전하게 처음부터 다시 (또는 루프 탈출)
                ImGui::PopID();
                continue;
            }
            ImGui::SameLine();
            ImGui::Checkbox(label.c_str(), &it->second);

            ImGui::PopID();
            ++it;
        }
        ImGui::TreePop();
    }

    // --- Float Parameters ---
    if (ImGui::TreeNode("Floats"))
    {
        for (auto it = _floatParams.begin(); it != _floatParams.end(); )
        {
            string label = Utils::ToString(it->first);
            ImGui::PushID(label.c_str());

            if (ImGui::Button("X")) {
                RemoveParameter(it->first, true);
                it = _floatParams.begin();
                ImGui::PopID();
                continue;
            }
            ImGui::SameLine();
            ImGui::Text("%s", label.c_str());
            ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::SetNextItemWidth(-1); // 남은 가로 영역 꽉 채우기
            ImGui::SliderFloat("##val", &it->second, 0.0f, 100.0f);

            ImGui::PopID();
            ++it;
        }
        ImGui::TreePop();
    }

    ImGui::Separator();

    // --- 파라미터 추가 버튼 ---
    if (ImGui::Button("+ Add Param"))
    {
        ImGui::OpenPopup("AddParamPopup");
    }

    if (ImGui::BeginPopup("AddParamPopup"))
    {
        static char newParamBuffer[64] = "";
        static int typeIdx = 0; // 0: Bool, 1: Float

        ImGui::InputText("Name", newParamBuffer, 64);
        ImGui::Combo("Type", &typeIdx, "Boolean\0Float\0");

        if (ImGui::Button("Add", ImVec2(120, 0)))
        {
            wstring wName = Utils::ToWString(newParamBuffer);
            if (!wName.empty())
            {
                if (typeIdx == 0 && _boolParams.find(wName) == _boolParams.end())
                    _boolParams[wName] = false;
                else if (typeIdx == 1 && _floatParams.find(wName) == _floatParams.end())
                    _floatParams[wName] = 0.0f;

                newParamBuffer[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
}

void AnimFSM::DrawStateDetailEditor(shared_ptr<AnimState> state)
{
    if (state == nullptr)
        return;

    // --- 이름 변경 UI ---
    char nameBuffer[128];
    // 현재 상태의 이름을 버퍼에 복사 (처음 선택했을 때만 혹은 매번)
    string currentName = Utils::ToString(state->GetName());
    //strcpy_s(nameBuffer, currentName.c_str());

    ImGui::Separator();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Edit State: %s", Utils::ToString(state->GetName()).c_str());

    // 애니메이션 클립 변경
    if (ImGui::BeginCombo("Animation Clip", state->GetAnimationClip() ? Utils::ToString(state->GetAnimationClip()->name).c_str() : "None"))
    {
        shared_ptr<Model> model = _animatorComponent.lock()->GetModel();
        if (nullptr != model)
        {
            // Model이 가진 모든 애니메이션 리스트 순회
            for (auto& clip : model->GetAnimations())
            {
                if (ImGui::Selectable(Utils::ToString(clip->name).c_str()))
                    state->SetAnimationClip(clip);
            }
            ImGui::EndCombo();
        }
    }

    ImGui::Separator();

    // Transition 추가 버튼
    if (ImGui::Button("Add Transition to..."))
    {
        ImGui::OpenPopup("AddTransitionPopup");
    }

    // --- Transition 추가 팝업 로직 ---
    if (ImGui::BeginPopup("AddTransitionPopup"))
    {
        ImGui::Text("Select Target State");
        ImGui::Separator();

        for (auto& [name, targetState] : _animStates)
        {
            // 자기 자신으로의 전이는 필요에 따라 제외하거나 포함 (보통 제외)
            if (targetState == state) continue;

            if (ImGui::Selectable(Utils::ToString(name).c_str()))
            {
                // 새로운 Transition 생성
                auto newTransition = make_shared<Transition>(shared_from_this());
                newTransition->SetToState(targetState);

                // 현재 상태에 추가
                state->Add_Transition(newTransition);

                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }

    // 현재 상태가 가진 Transition 리스트 출력 (삭제/수정용)
    ImGui::Spacing();
    ImGui::Text("[ Transitions ]");

    auto& transitions = state->GetTransitions();
    for (int i = 0; i < transitions.size(); ++i)
    {
        auto& trans = transitions[i];
        auto toState = trans->GetToState();
        if (!toState) continue;

        string label = "To: " + Utils::ToString(toState->GetName()) + "##" + to_string(i);
        if (ImGui::TreeNode(label.c_str()))
        {
            // Transition 속성 수정 (Duration, ExitTime 등)
            float duration = trans->GetTransitionDuration();
            if (ImGui::DragFloat("Duration", &duration, 0.01f, 0.0f, 2.0f))
                trans->SetTransitionDuration(duration);

            bool hasExit = trans->HasExitTime();
            if (ImGui::Checkbox("Has Exit Time", &hasExit))
                trans->SetHasExitTime(hasExit);

            // 삭제 버튼
            if (ImGui::Button("Delete Transition"))
            {
                transitions.erase(transitions.begin() + i);
                ImGui::TreePop();
                break; // 컨테이너 변형 시 루프 탈출
            }

            ImGui::TreePop();
        }
    }
}

void AnimFSM::DrawTransitionDetailEditor(shared_ptr<Transition> trans)
{
    if (!trans || !trans->GetToState()) return;

    ImGui::Begin("Transition Inspector"); // 별도 창 혹은 하단에 배치

    ImGui::TextColored(ImVec4(0, 1, 1, 1), "Transition to: %s",
        Utils::ToString(trans->GetToState()->GetName()).c_str());
    ImGui::Separator();

    // 기본 설정
    float duration = trans->GetTransitionDuration();
    if (ImGui::DragFloat("Transition Duration", &duration, 0.01f, 0.0f, 1.0f))
        trans->SetTransitionDuration(duration);

    bool hasExit = trans->HasExitTime();
    if (ImGui::Checkbox("Has Exit Time", &hasExit))
        trans->SetHasExitTime(hasExit);

    ImGui::Separator();

    // 조건부(Conditions) 편집
    ImGui::Text("Conditions");
    if (ImGui::Button("+ Add Condition"))
    {
        ImGui::OpenPopup("AddConditionPopup");
    }

    // 조건 목록 출력
    auto& conditions = trans->GetConditions();
    for (int i = 0; i < conditions.size(); ++i)
    {
        auto& cond = conditions[i];
        ImGui::PushID(i); // ID 충돌 방지

        ImGui::Text("[%s]", Utils::ToString(cond->_paramName).c_str());
        ImGui::SameLine();

        // 비교 연산자 표시 (Enum)
        const char* modes[] = { "Equal", "Greater", "Less", "NotEqual" };
        int currentMode = static_cast<int>(cond->_mode);
        if (ImGui::Combo("##Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
            cond->_mode = static_cast<ConditionMode>(currentMode);

        ImGui::SameLine();

        // 값 편집 (Float/Bool 타입 체크)
        if (auto fCond = dynamic_pointer_cast<FloatCondition>(cond))
            ImGui::DragFloat("##Value", &fCond->_value, 0.1f);
        else if (auto bCond = dynamic_pointer_cast<BoolCondition>(cond))
            ImGui::Checkbox("##Value", &bCond->_value);

        ImGui::SameLine();
        if (ImGui::Button("X"))
        {
            conditions.erase(conditions.begin() + i);
            ImGui::PopID();
            break;
        }

        ImGui::PopID();
    }

    // 조건 추가 팝업
    if (ImGui::BeginPopup("AddConditionPopup"))
    {
        // Blackboard(AnimFSM)에 등록된 파라미터 리스트를 보여줌
        for (auto& [name, val] : _floatParams)
        {
            if (ImGui::Selectable(Utils::ToString(name).c_str()))
            {
                auto newCond = make_shared<FloatCondition>();
                newCond->_paramName = name;
                trans->Add_Condition(newCond);
                ImGui::CloseCurrentPopup();
            }
        }
        for (auto& [name, val] : _boolParams)
        {
            if (ImGui::Selectable(Utils::ToString(name).c_str()))
            {
                auto newCond = make_shared<BoolCondition>();
                newCond->_paramName = name;
                trans->Add_Condition(newCond);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void AnimFSM::RemoveState(int nodeID)
{
    // ID(포인터)를 통해 대상 상태 찾기
    shared_ptr<AnimState> targetState = nullptr;
    wstring targetName = L"";

    for (auto it = _animStates.begin(); it != _animStates.end(); ++it)
    {
        if ((int)it->second.get() == nodeID)
        {
            targetState = it->second;
            targetName = it->first;
            break;
        }
    }

    if (!targetState) return;

    // 다른 모든 상태를 순회하며, 삭제될 상태를 가리키는 트랜지션 제거
    for (auto& [name, state] : _animStates)
    {
        auto& transitions = state->GetTransitions();
        transitions.erase(
            std::remove_if(transitions.begin(), transitions.end(),
                [&](shared_ptr<Transition> t) {
                    return t->GetToState() == targetState;
                }),
            transitions.end()
        );
    }

    // 메인 맵에서 삭제
    _animStates.erase(targetName);

    // 현재 실행 중인 상태였다면 초기화 (안전을 위해)
    if (_curAnimState == targetState)
        _curAnimState = nullptr;
}

void AnimFSM::RemoveParameter(const wstring& paramName, bool isFloat)
{
    // 파라미터 맵에서 제거
    if (isFloat) _floatParams.erase(paramName);
    else _boolParams.erase(paramName);

    // 모든 트랜지션을 순회하며 해당 파라미터를 참조하는 조건 삭제
    for (auto& [name, state] : _animStates)
    {
        for (auto& trans : state->GetTransitions())
        {
            auto& conditions = trans->GetConditions();
            conditions.erase(
                std::remove_if(conditions.begin(), conditions.end(),
                    [&](shared_ptr<Condition> cond) {
                        return cond->_paramName == paramName;
                    }),
                conditions.end()
            );
        }
    }
}