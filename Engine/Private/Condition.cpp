#include "pch.h"
#include "Condition.h"
#include "AnimFSM.h"
bool FloatCondition::IsSatisfied(shared_ptr<AnimFSM> fsm)
{
        float curFloat = fsm->GetFloat(_paramName);

        switch (_mode)
        {
        case ConditionMode::Equal:
            if (curFloat == _value)
                return true;
            break;
        case ConditionMode::Greater:
            if (curFloat < _value)
                return true;
            break;
        case ConditionMode::Less:
            if (curFloat > _value)
                return true;
            break;
        case ConditionMode::NotEqual:
            if (curFloat != _value)
                return true;
            break;
        default:
            break;
        }
        return false; // 결과 반환
}

bool BoolCondition::IsSatisfied(shared_ptr<AnimFSM> fsm)
{
    bool curBool = fsm->GetBool(_paramName);

    // bool은 ConditionMode에 상관없이 일치하면 true 리턴
    return curBool == _value;
}

bool TriggerCondition::IsSatisfied(shared_ptr<AnimFSM> fsm)
{
    bool curTriggerBool = fsm->GetTrigger(_paramName);

    // 리턴은 트리거값으로
    return curTriggerBool;
}
