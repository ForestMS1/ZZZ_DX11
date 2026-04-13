#pragma once

NS_BEGIN(Engine)

class Transition;
class AnimFSM;

class ENGINE_DLL AnimState : public enable_shared_from_this<AnimState>
{
public:
	AnimState(shared_ptr<AnimFSM> fsm);
	~AnimState();

public:
	virtual void Update();



public:
	void SetName(const wstring& name) { _stateName = name; }
	const wstring& GetName() { return _stateName; }

	void SetAnimationClip(shared_ptr<ModelAnimation> animation);
	shared_ptr<ModelAnimation> GetAnimationClip() { return _clip; }
	UINT GetAnimationClipIndex() { return _clipIndex; }

	void Add_Transition(shared_ptr<Transition> transition) { _transitions.push_back(transition); }
	vector<shared_ptr<Transition>>& GetTransitions() { return _transitions; }

private:
	wstring _stateName;
	wstring _clipName;
	shared_ptr<ModelAnimation> _clip;
	UINT _clipIndex = 0; // 모델에서의 애니메이션 Index 캐싱해놓음
	vector<shared_ptr<Transition>> _transitions;
	weak_ptr<AnimFSM> _animFSM;
};

NS_END