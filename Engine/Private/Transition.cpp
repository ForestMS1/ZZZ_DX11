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

    // Has Exit Time 체크 (애니메이션이 끝나야 넘어가는 경우)
    if (_hasExitTime == true)
    {
        return animator->IsCurrentAnimFinished();
    }

    //  모든 조건(Condition) 만족 여부 체크
    for (auto& condition : _conditions)
    {   
        if (!condition->IsSatisfied(fsm))
            return false;
    }

    return true;
}

//void Transition::Update()
//{
//
//}
