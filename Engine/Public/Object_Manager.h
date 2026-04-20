#pragma once

// 1.원형(CGameObject)를 복제한 사본객체들을 보관한다.
// 1_1. 레벨별로 구분해서.
// 1_2. 레벨안에서 사용자의 목적에 맞게 묶어서 관리한다.(Layer)

NS_BEGIN(Engine)

class Layer;
class LevelSaveLoader;

class ENGINE_DLL Object_Manager final
{
private:
	Object_Manager();
public:
	~Object_Manager();

public:
	// Object_Manager 초기화
	HRESULT		Initialize(uint32 iNumLevels);

	// 생명주기 함수
	void		BeginFrame(); // 매 프레임마다 Awake, Start 호출안된 오브젝트들 호출해주는 함수
	void		Update();
	void		LateUpdate();
	void		FixedUpdate();
	void		EndOfFrame(); // 오브젝트 삭제, Layer이동, 씬 전환 등 미뤄놨다가 여기서 처리

	//일단 레이어만 만들어서 등록할 수 있도록 함수 추가
	HRESULT		Add_Layer(uint32 iLayerLevelIndex, const wstring& strLayerTag);
	// 프로토타입->클론해서 레이어에 추가
	HRESULT		Add_GameObject_toLayer(uint32 iPrototypeLevelIndex, const wstring& strPrototypeTag,
		uint32 iLayerLevelIndex, const wstring& strLayerTag, void* pArg = nullptr);
	// 프로토타입->클론 없이 바로 레벨에 오브젝트 추가하는 함수
	HRESULT		Add_GameObject_toLayerNoClone(uint32 iLayerLevelIndex, const wstring & strLayerTag, shared_ptr<GameObject> pGameObject);
	// 레벨의 오브젝트 싹 비우기
	void		Clear(uint32 iClearLevelIndex);

	// 충돌 매니저에 현재레벨의 레이어 넘겨주기위해
	shared_ptr<Layer>					Find_CurrentLevel_Layer(const wstring& strLayerTag);
	shared_ptr<GameObject>				Find_GameObject_fromLayer(const wstring& strLayerTag, const wstring& objName);
	// 레벨 저장/불러오기를 위해 특정 레벨의 특정Layer의 gameObjectlist 넘겨주는 함수 만듦
	const list<shared_ptr<GameObject>>& Get_GameObjects(uint32 iLayerLevelIndex, const wstring& strLayerTag);


	// 카메라관리
	void	DisableCameras();
	void	firstFindCamOn();
	bool	isEveryCameraOff();


	// Gui 말고 코드레벨에서 Save-Load 가능하도록 인터페이스 열어줌
	void	SaveLevel(uint32 iLayerLevelIndex, const wstring& strLayerTag);
	void	LoadLevel(uint32 iLayerLevelIndex, const wstring& strLayerTag);


	//ImGui
	void	ShowHiearchy(const char** levelNames = nullptr);
	void	ShowInspector();
	void	RenderGizmo();

private:
	shared_ptr<Layer> Find_Layer(uint32 iLayerLevelIndex, const wstring& strLayerTag);

private:
	unique_ptr<map<const wstring, shared_ptr<Layer>>[]> _layerMaps = { nullptr };
	typedef map<const wstring, shared_ptr<Layer>> LAYERS;

	// 저장
	shared_ptr<LevelSaveLoader> _levelSaveLoader;

	uint32	_numLevels = { 0 };
	uint32 _currentLevelIndex = { 0 };

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

