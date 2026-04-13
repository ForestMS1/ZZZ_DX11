#pragma once


NS_BEGIN(Engine)

class ModelAnimator;
class AnimState;
class Transition;

class ENGINE_DLL AnimFSM : public enable_shared_from_this<AnimFSM>
{
public:
	AnimFSM(shared_ptr<ModelAnimator> animatorComponent);
	~AnimFSM();

	void Update();

	void ApplyTransition(shared_ptr<Transition> transition);
	void ChangeState(const wstring& stateName);

	shared_ptr<AnimState> GetCurAnimState() { return _curAnimState; }

	void Add_AnimState(const wstring& stateKey, shared_ptr<AnimState> state);

	//TweenDesc Property
	TweenDesc& GetTweenDesc();
	void SetTweenDesc(const TweenDesc& desc);

	shared_ptr<Transition> GetCurTransition() { return _curTransition; }

	void SetAnimator(shared_ptr<ModelAnimator> animatorComponent) { _animatorComponent = animatorComponent; }
	shared_ptr<ModelAnimator> GetAnimator() { return _animatorComponent.lock(); }

	bool GetBool(const wstring& paramName) { return _boolParams[paramName]; }
	void SetBool(const wstring& paramName, bool cond) { _boolParams[paramName] = cond; }
	void AddBool(const wstring& paramName, bool cond) { _boolParams.emplace(paramName, cond); }
	float GetFloat(const wstring& paramName) { return _floatParams[paramName]; }
	void SetFloat(const wstring& paramName, float value) { _floatParams[paramName] = value; }
	void AddFloat(const wstring& paramName, float value) { _floatParams.emplace(paramName, value); }
	void OnInspectorGUI();


	void Save(const string& fileName);
	void Load(const string& fileName, shared_ptr<ModelAnimator> animatorComponent);

private:
	void RenderParameters();
	void DrawStateDetailEditor(shared_ptr<AnimState> state);
	void HandleLinkCreation();
	void RenameState(const wstring& oldName, const wstring& newName);
	void DrawTransitionDetailEditor(shared_ptr<Transition> trans);
	void RemoveState(int nodeID);
	void RemoveParameter(const wstring& paramName, bool isFloat);
private:
	weak_ptr<ModelAnimator> _animatorComponent;

	shared_ptr<AnimState> _curAnimState;
	shared_ptr<Transition> _curTransition;
	unordered_map<wstring, shared_ptr<AnimState>> _animStates;

	// 파라미터
	map<wstring, bool> _boolParams;
	map<wstring, float> _floatParams;
};

NS_END
