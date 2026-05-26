#pragma once
#include "Component.h"
#include "AnimFSM.h"
NS_BEGIN(Engine)

//class AnimFSM;

//		     Bone1  Bone2  Bone3 ...  Bone350
//	Frame1   [SRT]  [SRT]  [SRT]  ...  [SRT]
//  Frame2   [SRT]
//  Frame3   [SRT]
//	...		 ...
//	Frame500 [SRT]
//
class ENGINE_DLL ModelAnimator : public Component
{
public:
	ModelAnimator();
	ModelAnimator(shared_ptr<Shader> shader);
	~ModelAnimator();

public:
	// 생명주기 함수
	virtual void Awake() override;
	virtual void Update() override;
	virtual HRESULT Render() override;
	HRESULT RenderShadow() override;

	// Model Property
	void SetModel(shared_ptr<Model> model);
	shared_ptr<Model> GetModel() { return _model; }
	// Shader Property
	
	shared_ptr<Shader> GetShader() { return _shader; }
	// Technique Property
	void SetTechnique(uint8 technique) { _techniqueIndex = technique; }
	uint8 GetTechnique() { return _techniqueIndex; }
	// Pass Property
	void SetPass(uint8 pass) { _pass = pass; }
	uint8 GetPass() { return _pass; }

	// Renderer RenderGroup Property
	RENDERGROUP GetCurRenderGroup() { return _renderGroup; }
	void SetCurRenderGroup(RENDERGROUP eRenderGroup) { _renderGroup = eRenderGroup; }

	//AnimFSM Property
	shared_ptr<AnimFSM> GetFSM() { return _animFSM; }
	void SetFSM(shared_ptr<AnimFSM> fsm) { _animFSM = fsm; fsm->SetAnimator(SHARED_THIS(ModelAnimator)); }

	// TweenDesc Property
	TweenDesc& GetTweenDesc() { return _tweenDesc; }
	void SetTweenDesc(const TweenDesc& desc) { _tweenDesc = desc; }

	// 키프레임별 Bone의 SRT
	const vector<AnimTransform>& GetAnimTransforms() { return _model->GetAnimTransforms(); }
	const vector<vector<Matrix>>& GetRootAnimTransform() { return _model->GetRootAnimTransform(); }

	// 현재 애니메이션 진행률
	float GetProgress();
	// 현재 애니메이션이 끝났는가
	bool IsCurrentAnimFinished();
	// 현재 애니메이션State 이름 얻기
	wstring GetCurAnimStateName();

	// FSM의 bool 파라미터 Property
	bool GetBool(const wstring& paramName) { return _animFSM->GetBool(paramName); }
	void SetBool(const wstring& paramName, bool cond) { _animFSM->SetBool(paramName, cond); }
	void AddBool(const wstring& paramName, bool cond) { _animFSM->AddBool(paramName, cond); }

	// FSM의 float 파라미터 Property
	float GetFloat(const wstring& paramName) { return _animFSM->GetFloat(paramName); }
	void SetFloat(const wstring& paramName, float value) { _animFSM->SetFloat(paramName, value); }
	void AddFloat(const wstring& paramName, float value) { _animFSM->AddFloat(paramName, value); }

	// FSM의 trigger 파라미터 Property
	bool GetTrigger(const wstring& paramName) { return _animFSM->GetTrigger(paramName); }
	void SetTrigger(const wstring& paramName, float value = true) { _animFSM->SetTrigger(paramName, value); }
	void AddTrigger(const wstring& paramName, float value = true) { _animFSM->AddTrigger(paramName, value); }


	//ImGui
	virtual void OnInspectorGUI() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) { return nullptr; }

private:
	KeyframeDesc _keyframeDesc;
	TweenDesc _tweenDesc;
	shared_ptr<AnimFSM> _animFSM;

private:
	shared_ptr<Shader>	_shader;
	shared_ptr<Shader> _shadowShader = nullptr;
	uint8				_techniqueIndex = 0;
	uint8				_pass = 0;
	// 애니메이션, 메쉬, 머터리얼을 가지고 있는 Model
	shared_ptr<Model>	_model;

	// 렌더러 그룹
	RENDERGROUP _renderGroup = RENDERGROUP::PRIORITY; // 따로 설정 안했으면 PRIORITY
};
NS_END