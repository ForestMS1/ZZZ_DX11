#pragma once

NS_BEGIN(Engine)

class Transition;
class AnimFSM;

class ENGINE_DLL AnimState : public enable_shared_from_this<AnimState>
{
public:
	AnimState(shared_ptr<AnimFSM> fsm);
	~AnimState();

// TODO : ModelAnimator 컴포넌트의 Update 일부를 여기로 빼올까 고민 중
//public:
//	void Update();

public:
	// AnimFSM에서 Add하면서 키값과 동일하게 이름이 Set 된다
	void SetName(const wstring& name) { _stateName = name; }
	const wstring& GetName() { return _stateName; }

	// AnimationClip Property
	void SetAnimationClip(shared_ptr<ModelAnimation> animation); // animation이 Model에서 몇번째 인덱스인지 캐싱해둔다.
	shared_ptr<ModelAnimation> GetAnimationClip() { return _clip; }

	UINT GetAnimationClipIndex() { return _clipIndex; } // animation이 Model에서 몇번째 인덱스인지 캐싱해둔다.

	void Add_Transition(shared_ptr<Transition> transition) { _transitions.push_back(transition); }
	vector<shared_ptr<Transition>>& GetTransitions() { return _transitions; }

private:
	// 주인 FSM
	weak_ptr<AnimFSM> _animFSM;
	// AnimFSM에서 Add하면서 키값과 동일하게 이름이 Set 된다
	wstring _stateName;
	// 실제 애니메이션 이름
	wstring _clipName;
	// 애니메이션
	shared_ptr<ModelAnimation> _clip;
	// 모델에서의 애니메이션 Index 캐싱해놓음
	UINT _clipIndex = 0;
	// AnimState에서 뻗어 나가는 Transition들
	vector<shared_ptr<Transition>> _transitions;
};

NS_END