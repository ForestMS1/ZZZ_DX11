#pragma once


NS_BEGIN(Engine)

#define ANYSTATE L"AnyState"

class ModelAnimator;
class AnimState;
class Transition;

class ENGINE_DLL AnimFSM : public enable_shared_from_this<AnimFSM>
{
public:
	AnimFSM(shared_ptr<ModelAnimator> animatorComponent);
	~AnimFSM();

public:
	void Update();
	void ApplyTransition(shared_ptr<Transition> transition);

	// ModelAnimator에서 다음 애니메이션으로 넘어갈 떄 호출
	void ChangeState(const wstring& stateName);

	shared_ptr<AnimState> GetCurAnimState() { return _curAnimState; }
	// Key와 state의 이름을 동일하게 추가한다.
	void Add_AnimState(const wstring& stateKey, shared_ptr<AnimState> state);

	// TweenDesc Property
	TweenDesc& GetTweenDesc();
	void SetTweenDesc(const TweenDesc& desc);

	// 현재 Tween되어지고있는 Transition Getter
	shared_ptr<Transition> GetCurTransition() { return _curTransition; }

	// 주인 Animator 컴포넌트 Property
	void SetAnimator(shared_ptr<ModelAnimator> animatorComponent) { _animatorComponent = animatorComponent; }
	shared_ptr<ModelAnimator> GetAnimator() { return _animatorComponent.lock(); }

	// FSM의 bool 파라미터 Property
	bool GetBool(const wstring& paramName) { return _boolParams.at(paramName); }
	void SetBool(const wstring& paramName, bool cond) { _boolParams.at(paramName) = cond; }
	void AddBool(const wstring& paramName, bool cond) { _boolParams.emplace(paramName, cond); }

	// FSM의 float 파라미터 Property
	float GetFloat(const wstring& paramName) { return _floatParams.at(paramName); }
	void SetFloat(const wstring& paramName, float value) { _floatParams.at(paramName) = value; }
	void AddFloat(const wstring& paramName, float value) { _floatParams.emplace(paramName, value); }

	// FSM의 trigger 파라미터 Property
	bool GetTrigger(const wstring& paramName) { return _triggerParams.at(paramName); }
	void SetTrigger(const wstring& paramName, float value = true) { _triggerParams.at(paramName) = value; }
	void AddTrigger(const wstring& paramName, float value = true) { _triggerParams.emplace(paramName, value); }

	map<wstring, bool>& GetBoolMap() { return _boolParams; }
	map<wstring, float>& GetFloatMap() { return _floatParams; }
	map<wstring, bool>& GetTriggerMap() { return _triggerParams; }
	// 네트워크 동기화용 함수 추가
	void UpdateParamByHash(uint32_t paramHash, float value);

	void OnInspectorGUI();

	// FSM Save & Load XML파일
	void Save(const string& fileName);
	void Load(const string& fileName, shared_ptr<ModelAnimator> animatorComponent);
	wstring GetFileName() { return _fileName; };

	//Gui용 함수들
private:
	// 파라미터 목록 보여줌
	void RenderParameters();
	// AnimState 상세히 보여줌
	void DrawStateDetailEditor(shared_ptr<AnimState> state);
	// Transition 추가/삭제
	void HandleLinkCreation();
	// Transition 클릭 시 상세정보
	void DrawTransitionDetailEditor(shared_ptr<Transition> trans);
	// AnimState 삭제
	void RemoveState(int nodeID);
	// 파라미터 삭제
	void RemoveParameter(const wstring& paramName, bool isFloat);
private:
	// 주인 Animaotr 컴포넌트
	weak_ptr<ModelAnimator> _animatorComponent;

	// 조건 충족되면 바로 트랜지션 발동하는 Any State
	shared_ptr<AnimState> _anyState;

	// 현재 진행중인 AnimState
	shared_ptr<AnimState> _curAnimState;
	// 현재 Tween되어 지고있는 Transition
	shared_ptr<Transition> _curTransition;
	// AnimState들 unordered_map으로 관리
	unordered_map<wstring, shared_ptr<AnimState>> _animStates;

	// xml 파일이름
	wstring _fileName;

	// 파라미터
	map<wstring, bool> _boolParams;
	map<wstring, float> _floatParams;
	map<wstring, bool> _triggerParams;
};

NS_END
