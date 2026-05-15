#include "pch.h"
#include "Transition.h"
#include "AnimFSM.h"
#include "ModelAnimator.h"
#include "Condition.h"
#include "AnimState.h"
Transition::Transition(shared_ptr<AnimFSM> fsm)
    : _animFSM(fsm)
{
}
Transition::~Transition()
{
}
bool Transition::CanTransition()
{
    auto fsm = _animFSM.lock();
    auto animator = fsm->GetAnimator();
    if (!fsm || !animator) 
		return false;

    // Has Exit Time 체크되어 있으면 _exitTime까지 버틴다
    if (_hasExitTime == true && animator->GetProgress() < _exitTime)
    {
        return false;
    }

    //----------------------------- Has Exit Time 체크x or _exitTime 통과 -------------------------------


    //  모든 조건(Condition) 만족 여부 체크
    for (auto& condition : _conditions)
    {
        // 조건 하나라도 안맞으면 전환x
        if (!condition->IsSatisfied(fsm))
            return false;
    }


    return true;
}

//void Transition::Update()
//{
//
//}
