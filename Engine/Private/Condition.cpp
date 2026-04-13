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

    return curBool == _value;
}
