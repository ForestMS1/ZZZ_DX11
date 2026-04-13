#pragma once

NS_BEGIN(Engine)

class AnimFSM;
class AnimState;
class Condition;

class ENGINE_DLL Transition
{
public:
    Transition(shared_ptr<AnimFSM> fsm);
    ~Transition();
public:
    // 조건들을 체크하여 전이가 가능한지 반환
    bool CanTransition();

    void Update();

    // Property
    void SetToState(shared_ptr<AnimState> state) { _toState = state; }
    shared_ptr<AnimState> GetToState() { return _toState.lock(); }
    void SetTransitionDuration(float duration) { _transitionDuration = duration; }
    float GetTransitionDuration() { return _transitionDuration; }
    bool HasExitTime() { return _hasExitTime; }
    void SetHasExitTime(bool hasExit) { _hasExitTime = hasExit; }
    void Add_Condition(shared_ptr<Condition> condition) { _conditions.push_back(condition); }

    vector<shared_ptr<Condition>>& GetConditions() { return _conditions; }
private:
    weak_ptr<AnimFSM> _animFSM;
    weak_ptr<AnimState> _toState; // 넘어갈 AnimState
    float _transitionDuration = 0.5f; // 블렌딩 시간

    bool _hasExitTime = false;       // 애니메이션이 일정 비율 이상 재생되어야 넘어가는지
    float _exitTimeValue = 1.f;     // 재생 비율 (0.0 ~ 1.0)

    // 여러 조건을 관리 (예: Speed > 0.5 && IsGrounded == true)
    vector<shared_ptr<Condition>> _conditions;
};

NS_END