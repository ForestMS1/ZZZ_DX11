#include "pch.h"
#include "AnimState.h"
#include "AnimFSM.h"
#include "Transition.h"
#include "ModelAnimator.h"
AnimState::AnimState(shared_ptr<AnimFSM> fsm)
	: _animFSM(fsm)
{
}

AnimState::~AnimState()
{
}

//void AnimState::Update()
//{
//
//}

void AnimState::SetAnimationClip(shared_ptr<ModelAnimation> animation)
{
	_clip = animation;

	_clipName = animation->name;
	// ÀÎµ¦½º Ä³½ÌÇØ³õ´Â´Ù.
	_clipIndex = _animFSM.lock()->GetAnimator()->GetModel()->GetAnimationIndexByName(animation->name);
}