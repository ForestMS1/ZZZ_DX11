#pragma once

// 1.원형(CGameObject)를 복제한 사본객체들을 보관한다.
// 1_1. 레벨별로 구분해서.
// 1_2. 레벨안에서 사용자의 목적에 맞게 묶어서 관리한다.(Layer)

NS_BEGIN(Engine)

class Layer;

class ENGINE_DLL Object_Manager final
{
private:
	Object_Manager();
public:
	~Object_Manager();

public:
	HRESULT Initialize(uint32 iNumLevels);

	void BeginFrame(); // 매 프레임마다 Awake, Start 호출안된 오브젝트들 호출해주는 함수
	void Update();
	void LateUpdate();
	void FixedUpdate();

	HRESULT Add_GameObject_toLayer(uint32 iPrototypeLevelIndex, const wstring& strPrototypeTag,
		uint32 iLayerLevelIndex, const wstring& strLayerTag, void* pArg = nullptr);
	void Clear(uint32 iClearLevelIndex);
	shared_ptr<GameObject> Find_GameObject_fromLayer(const wstring& strLayerTag, const wstring& objName);

	//ImGui
	void ShowHiearchy();
	void ShowInspector();
	void RenderGizmo();

private:
	uint32 _numLevels = { 0 };
	unique_ptr<map<const wstring, unique_ptr<Layer>>[]> _layerMaps = { nullptr };
	typedef map<const wstring, unique_ptr<Layer>> LAYERS;

	uint32 _currentLevelIndex = { 0 };

private:
	Layer* Find_Layer(uint32 iLayerLevelIndex, const wstring& strLayerTag);

private:
	// ImGui 용 변수
	shared_ptr<GameObject> _selectedObject;
	ImGuizmo::OPERATION _currentOp = ImGuizmo::TRANSLATE;
private:
	// ImGui 용 함수
	void RenderTransformTree(shared_ptr<GameObject> pGameObject);

public:
	static unique_ptr<Object_Manager> Create(uint32 iNumLevels);
};

NS_END

