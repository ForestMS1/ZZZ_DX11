#include "pch.h"
#include "Object_Manager.h"
#include "Layer.h"

// ------------ Component ----------------
#include "AABBCollider.h"
#include "ModelAnimator.h"
//--------------------------------------
#include "GameInstance.h"
#include "GameObject.h"
#include "LevelSaveLoader.h"
Object_Manager::Object_Manager()
{

}

Object_Manager::~Object_Manager()
{
}

HRESULT Object_Manager::Initialize(uint32 iNumLevels)
{
	_layerMaps = unique_ptr<LAYERS[]>(new LAYERS[iNumLevels]);


	_numLevels = iNumLevels;

	_levelSaveLoader = make_shared<LevelSaveLoader>();

	return S_OK;
}


void Object_Manager::BeginFrame()
{
	_currentLevelIndex = GameInstance::Get().GetCurrentLevelIndex();

	for (auto& pair : _layerMaps[_currentLevelIndex])
	{
		pair.second->BeginFrame();
	}

}

void Object_Manager::Update()
{
	for (auto& pair : _layerMaps[_currentLevelIndex])
	{
		pair.second->Update();
	}
}

void Object_Manager::LateUpdate()
{
	for (auto& pair : _layerMaps[_currentLevelIndex])
	{
		pair.second->LateUpdate();
	}
}

void Object_Manager::FixedUpdate()
{
	for (auto& pair : _layerMaps[_currentLevelIndex])
	{
		pair.second->FixedUpdate();
	}
}

void Object_Manager::EndOfFrame()
{
	for (auto& pair : _layerMaps[_currentLevelIndex])
	{
		pair.second->EndOfFrame();
	}
}

HRESULT Object_Manager::Add_Layer(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == _layerMaps || iLayerLevelIndex >= _numLevels)
		return E_FAIL;

	shared_ptr<Layer> pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (pLayer != nullptr)
		return E_FAIL;

	unique_ptr<Layer> pNewLayer = Layer::Create();
	if (nullptr == pNewLayer)
		return E_FAIL;

	_layerMaps[iLayerLevelIndex].emplace(strLayerTag, std::move(pNewLayer));

	return S_OK;
}

HRESULT Object_Manager::Add_GameObject_toLayer(uint32 iPrototypeLevelIndex, const wstring& strPrototypeTag, uint32 iLayerLevelIndex, const wstring& strLayerTag, void* pArg)
{
 	if (nullptr == _layerMaps || iLayerLevelIndex >= _numLevels)
		return E_FAIL;

	shared_ptr<GameObject> pGameObject = dynamic_pointer_cast<GameObject>(GameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag));
	if (nullptr == pGameObject)
		return E_FAIL;

	// 현재 객체를 추가하려고 하는 레이어가 없다면
	// 새로 레이어를 만들어 추가해준다.
	shared_ptr<Layer> pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		unique_ptr<Layer> pNewLayer = Layer::Create();
		if (nullptr == pNewLayer)
			return E_FAIL;
		
		if (FAILED(pNewLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		_layerMaps[iLayerLevelIndex].emplace(strLayerTag, std::move(pNewLayer));
	}
	else
	{
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT Object_Manager::Add_GameObject_toLayerNoClone(uint32 iLayerLevelIndex, const wstring& strLayerTag, shared_ptr<GameObject> pGameObject)
{
	if (nullptr == _layerMaps || iLayerLevelIndex >= _numLevels)
		return E_FAIL;

	// 현재 객체를 추가하려고 하는 레이어가 없다면
	// 새로 레이어를 만들어 추가해준다.
	shared_ptr<Layer> pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		unique_ptr<Layer> pNewLayer = Layer::Create();
		if (nullptr == pNewLayer)
			return E_FAIL;

		if (FAILED(pNewLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		_layerMaps[iLayerLevelIndex].emplace(strLayerTag, std::move(pNewLayer));
	}
	else
	{
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;
	}

	return S_OK;
}

void Object_Manager::Clear(uint32 iClearLevelIndex)
{
	if (iClearLevelIndex >= _numLevels ||
		nullptr == _layerMaps)
	{
		MSG_BOX("Object_Manager Clear Failed!");
		return;
	}

	for (auto& pair : _layerMaps[iClearLevelIndex])
	{
		pair.second.reset();
	}
	_layerMaps[iClearLevelIndex].clear();


	//imgui용
	_selectedObject = nullptr;
}

shared_ptr<GameObject> Object_Manager::Find_GameObject_fromLayer(const wstring& strLayerTag, const wstring& objName)
{
	shared_ptr<Layer> pLayer = Find_Layer(_currentLevelIndex, strLayerTag);
	for (const auto& gameObject : pLayer->Get_GameObjects())
	{
		if (gameObject->GetName() == objName)
			return gameObject;
	}
	return nullptr;
}

shared_ptr<Layer> Object_Manager::Find_CurrentLevel_Layer(const wstring& strLayerTag)
{
	auto iter = _layerMaps[_currentLevelIndex].find(strLayerTag);
	if (iter == _layerMaps[_currentLevelIndex].end())
		return nullptr;

	return iter->second;
}

const vector<shared_ptr<GameObject>>& Object_Manager::Get_GameObjects(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	// 레이어를 찾지 못했을 때 안전하게 반환할 빈 리스트
	static const vector<shared_ptr<GameObject>> emptyList;

	shared_ptr<Layer> pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		return emptyList;
	}

	return pLayer->Get_GameObjects();
}

void Object_Manager::SaveLevel(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	if (_levelSaveLoader)
		_levelSaveLoader->Save(iLayerLevelIndex, strLayerTag);
}

void Object_Manager::LoadLevel(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	if (_levelSaveLoader)
		_levelSaveLoader->Load(iLayerLevelIndex, strLayerTag);
}

void Object_Manager::ShowHiearchy(const char** levelNames)
{
	ImGui::Begin("Scene Hierarchy");

	if(levelNames == nullptr)
		ImGui::Text("Current Level: %d", _currentLevelIndex);
	else
		ImGui::Text("Current Level: %s", levelNames[_currentLevelIndex]);

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::Separator();

	auto& currentLayerMap = _layerMaps[_currentLevelIndex];

	for (auto& pair : currentLayerMap)
	{
		const wstring& layerTag = pair.first;
		Layer* pLayer = pair.second.get();

		string tagStr = Utils::ToString(layerTag);

		bool isNodeOpen = ImGui::TreeNodeEx(tagStr.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen);

		// --- 우클릭 오브젝트 추가 로직 ---
		// BeginPopupContextItem은 바로 직전에 호출된 위젯(TreeNode)을 대상으로 우클릭을 감지
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create Empty Object"))
			{
				shared_ptr<GameObject> newObj = GAME.CreateFromFactory(L"GameObject");
				// 리스트에 추가 (레이어 등록)
				GAME.Add_GameObject_toLayerNoClone(_currentLevelIndex, layerTag, newObj);
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropTarget())
		{
			// 모델 리소스 매니저로부터 드래그 온 경우
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_MODEL"))
			{
				const wstring& modelKey = *(const wstring*)payload->Data;

				// 새로운 게임오브젝트 생성
				auto pNewObj = make_shared<GameObject>(DEVICE, CONTEXT);
				pNewObj->SetName(modelKey);

				// ModelAnimator 컴포넌트 추가 및 모델 설정
				auto pModel = GAME.GetResource<Model>(modelKey);
				if (pModel->GetAnimationCount() > 0)
				{
					auto shader = pModel->GetMaterials().front()->GetShader();
					auto pAnimator = make_shared<ModelAnimator>(shader);
					if (pModel)
					{
						pAnimator->SetModel(pModel);
						pNewObj->AddComponent(pAnimator);
						pNewObj->Initialize();

						// 현재 레이어에 직접 추가
						pLayer->Add_GameObject(pNewObj);
					}
				}
				else // 애니메이션이 없는 모델이면 ModelRenderer붙여줌
				{
					auto shader = pModel->GetMaterials().front()->GetShader();
					auto pMeshRenderer = make_shared<ModelRenderer>(shader);
					if (pModel)
					{
						pMeshRenderer->SetModel(pModel);
						pNewObj->AddComponent(pMeshRenderer);
						pNewObj->Initialize();

						//현재 레이어에 직접 추가
						pLayer->Add_GameObject(pNewObj);
					}
				}
			}

			// 레이어 노드 자체를 드롭 타겟으로 설정 (부모 해제 영역)
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TRANSFORM_NODE"))
			{
				GameObject* draggedObj = *(GameObject**)payload->Data;
				// 부모를 nullptr로 설정하여 최상위로 올림
				draggedObj->GetTransform()->SetParent(nullptr);
			}
			ImGui::EndDragDropTarget();
		}
		if (isNodeOpen)
		{
			const vector<shared_ptr<GameObject>>& objects = pLayer->Get_GameObjects();

			for (auto& pGameObject : objects)
			{
				// 최상위 부모(부모가 없는 오브젝트)만 먼저 호출
				// 자식들은 RenderTransformTree 내에서 재귀적으로 그려짐
				auto pTransform = pGameObject->GetTransform();
				if (pTransform && pTransform->HasParent() == false)
				{
					RenderTransformTree(pGameObject);
				}
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void Object_Manager::RenderTransformTree(shared_ptr<GameObject> pGameObject)
{
	if (!pGameObject) return;

	auto pTransform = pGameObject->GetTransform();
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (pTransform->GetChildrenTransform().empty()) flags |= ImGuiTreeNodeFlags_Leaf;
	if (_selectedObject == pGameObject) flags |= ImGuiTreeNodeFlags_Selected;

	bool isOpen = ImGui::TreeNodeEx((void*)pGameObject.get(), flags, Utils::ToString(pGameObject->GetName()).c_str());

	// --- 우클릭 삭제 로직 추가 ---
	// BeginPopupContextItem은 바로 직전에 호출된 위젯(TreeNode)을 대상으로 우클릭을 감지
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete Object"))
		{
			pGameObject->SetLifeState(LIFESTATE::REMOVE);
		}
		ImGui::EndPopup();
	}

	// --- 드래그 소스 (내보내기) ---
	if (ImGui::BeginDragDropSource())
	{
		// 이동시킬 오브젝트의 주소를 보냄
		GameObject* pRawPtr = pGameObject.get();
		ImGui::SetDragDropPayload("TRANSFORM_NODE", &pRawPtr, sizeof(GameObject*));

		ImGui::Text("Move %s", Utils::ToString(pGameObject->GetName()).c_str());
		ImGui::EndDragDropSource();
	}

	// --- 드롭 타겟 (자식으로 받기) ---
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TRANSFORM_NODE"))
		{
			GameObject* draggedObj = *(GameObject**)payload->Data;

			// 자기 자신에게 드롭하거나, 이미 부모인 경우 제외 (순환 참조 방지 로직 필요)
			if (draggedObj != pGameObject.get())
			{
				// 기존 부모에서 제거하고 새로운 부모 설정
				// 실제 구현 시 pTransform->SetParent() 내부에서 
				// 기존 부모의 _children 리스트에서도 제거하는 처리가 되어있어야 함
				pTransform->AddChild(draggedObj->GetTransform());
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked()) _selectedObject = pGameObject;

	if (isOpen)
	{
		for (auto& pChildTransform : pTransform->GetChildrenTransform())
		{
			auto pChildObject = pChildTransform->GetGameObject();
			if (pChildObject) RenderTransformTree(pChildObject);
		}
		ImGui::TreePop();
	}
}

void Object_Manager::ShowInspector()
{
	ImGui::Begin("Inspector");

	if (ImGui::Button("Level Save"))
	{
		for (auto& pair : _layerMaps[_currentLevelIndex])
		{
			_levelSaveLoader->Save(_currentLevelIndex, pair.first);
		}

		// 팝업을 열라고 명령만 내림
		ImGui::OpenPopup("SaveCheck");
	}

	// 팝업 렌더링 코드는 버튼 검사 외부(매 프레임 실행되는 영역)에 위치해야 함
	if (ImGui::BeginPopupModal("SaveCheck", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Save Complete!\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup(); // BeginPopupModal을 썼다면 반드시 EndPopup을 호출해야 함
	}
	ImGui::SameLine();
	if (ImGui::Button("Level Load"))
	{
		for (auto& pair : _layerMaps[_currentLevelIndex])
		{
			_levelSaveLoader->Load(_currentLevelIndex, pair.first);
		}
		// 팝업을 열라고 명령만 내림
		ImGui::OpenPopup("LoadCheck");
	}
	// 팝업 렌더링 코드는 버튼 검사 외부(매 프레임 실행되는 영역)에 위치해야 함
	if (ImGui::BeginPopupModal("LoadCheck", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Load Complete!\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup(); // BeginPopupModal을 썼다면 반드시 EndPopup을 호출해야 함
	}
	ImGui::Separator();

	if (_selectedObject == nullptr)
	{
		ImGui::TextColored(ImVec4(1, 1, 1, 0.5f), "Select an object to view details.");
		ImGui::End();
		return;
	}

	// --- 기본 정보 (이름 등) ---
	char buf[256];
	char classBuf[256];
	string currentName = Utils::ToString(_selectedObject->GetName());
	string currentClassName = Utils::ToString(_selectedObject->Get_ClassName());
	strcpy_s(buf, currentName.c_str());
	strcpy_s(classBuf, currentClassName.c_str());

	if (ImGui::InputText("Name", buf, 256))
	{
		_selectedObject->SetName(Utils::ToWString(buf)); // 수정 시 즉시 반영
		//_selectedObject->Set_ClassName(Utils::ToWString(buf));
	}
	if (ImGui::InputText("ClassName", classBuf, 256))
	{
		_selectedObject->Set_ClassName(Utils::ToWString(classBuf));// 수정 시 즉시 반영
	}

	// UUID
	RPC_CSTR id = nullptr;
	UUID tempId = _selectedObject->GetId();
	if (UuidToStringA(&tempId, &id) == RPC_S_OK)
	{
		ImGui::Text("UUID:");
		ImGui::SameLine();
		ImGui::Text((char*)id);
		RpcStringFreeA(&id);
	}

	ImGui::Separator();

	// --- 컴포넌트 정보 ---
	auto pTransform = _selectedObject->GetTransform();
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// --- ImGuizmo Operation 모드 전환 버튼 ---
		ImGui::Text("Gizmo Mode:");
		ImGui::SameLine();

		// 1. Translate
		bool isTranslate = (_currentOp == ImGuizmo::TRANSLATE);
		if (isTranslate) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f)); // 선택 시 파란색 하이라이트
		if (ImGui::Button("T##Trans"))
		{
			_currentOp = ImGuizmo::TRANSLATE;
		}
		if (isTranslate) ImGui::PopStyleColor();
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Translate");

		ImGui::SameLine();

		// 2. Rotate
		bool isRotate = (_currentOp == ImGuizmo::ROTATE);
		if (isRotate) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
		if (ImGui::Button("R##Rot"))
		{
			_currentOp = ImGuizmo::ROTATE;
		}
		if (isRotate) ImGui::PopStyleColor();
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Rotate");

		ImGui::SameLine();

		// 3. Scale
		bool isScale = (_currentOp == ImGuizmo::SCALE);
		if (isScale) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
		if (ImGui::Button("S##Scale"))
		{
			_currentOp = ImGuizmo::SCALE;
		}
		if (isScale) ImGui::PopStyleColor();
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Scale");

		ImGui::Separator();

		// --- Position ---
		if (!pTransform->HasParent())
		{
			Vec3 pos = pTransform->GetPosition();
			// DragFloat3의 너비를 조절하여 버튼 공간 확보
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
			if (ImGui::DragFloat3("Position", (float*)&pos, 0.1f, 0, 0))
			{
				pTransform->SetLocalPosition(pos);
			}
			ImGui::PopItemWidth();

			ImGui::SameLine(); // 같은 줄에 배치
			if (ImGui::Button("R##ResetPos")) // ## 뒤의 이름은 ID로 작동하여 중복 방지
			{
				pTransform->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Position");

			// --- Rotation ---
			Vec3 rot = pTransform->GetRotation();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
			if (ImGui::DragFloat3("Rotation", (float*)&rot, 0.5f))
			{
				pTransform->SetLocalRotation(rot);
			}
			ImGui::PopItemWidth();

			ImGui::SameLine();
			if (ImGui::Button("R##ResetRot"))
			{
				pTransform->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Rotation");

			// --- Scale ---
			Vec3 scale = pTransform->GetScale();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
			if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f, -1000, 1000))
			{
				pTransform->SetLocalScale(scale);
			}
			ImGui::PopItemWidth();
		}
		else
		{
			Vec3 pos = pTransform->GetLocalPosition();
			// DragFloat3의 너비를 조절하여 버튼 공간 확보
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
			if (ImGui::DragFloat3("Position", (float*)&pos, 0.1f, 0, 0))
			{
				pTransform->SetLocalPosition(pos);
			}
			ImGui::PopItemWidth();

			ImGui::SameLine(); // 같은 줄에 배치
			if (ImGui::Button("R##ResetPos")) // ## 뒤의 이름은 ID로 작동하여 중복 방지
			{
				pTransform->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Position");
	
			// --- Rotation ---
			Vec3 rot = pTransform->GetLocalRotation();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
			if (ImGui::DragFloat3("Rotation", (float*)&rot, 0.5f))
			{
				pTransform->SetLocalRotation(rot);
			}
			ImGui::PopItemWidth();

			ImGui::SameLine();
			if (ImGui::Button("R##ResetRot"))
			{
				pTransform->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Rotation");

			// --- Scale ---
			Vec3 scale = pTransform->GetLocalScale();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
			if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f, -1000, 1000))
			{
				pTransform->SetLocalScale(scale);
			}
			ImGui::PopItemWidth();
		}
		

		ImGui::SameLine();
		if (ImGui::Button("R##ResetScale"))
		{
			pTransform->SetScale(Vec3(1.f, 1.f, 1.f));
		}
	}

	// ------------------------------------------- AddComponent -----------------------------------------------------
	if (ImGui::Button("Add Component", ImVec2(-1, 0))) // 너비를 가득 채우려면 ImVec2(-1, 0)
	{
		ImGui::OpenPopup("ComponentSearchPopup");
	}

	// 팝업 내부 로직
	if (ImGui::BeginPopup("ComponentSearchPopup"))
	{
		ImGui::TextDisabled("Fixed Components");
		ImGui::Separator();

		if (_selectedObject->GetCamera() == nullptr && ImGui::Selectable("Camera"))
		{
			shared_ptr<Camera> camera = make_shared<Camera>();
			_selectedObject->AddComponent(camera);
		}

		if (_selectedObject->GetCollider() == nullptr && ImGui::Selectable("AABB Collider"))
		{
			shared_ptr<AABBCollider> collider = make_shared<AABBCollider>();
			_selectedObject->AddComponent(collider);
		}

		if (_selectedObject->GetCollider() == nullptr && ImGui::Selectable("OBB Collider"))
		{
			shared_ptr<OBBCollider> collider = make_shared<OBBCollider>();
			_selectedObject->AddComponent(collider);
		}

		if (_selectedObject->GetCollider() == nullptr && ImGui::Selectable("Sphere Collider"))
		{
			shared_ptr<SphereCollider> collider = make_shared<SphereCollider>();
			_selectedObject->AddComponent(collider);
		}

		if (_selectedObject->GetLight() == nullptr && ImGui::Selectable("Light"))
		{
			shared_ptr<Light> light = make_shared<Light>();
			_selectedObject->AddComponent(light);
		}

		if (_selectedObject->GetBillBoard() == nullptr && ImGui::Selectable("BillBoard"))
		{
			shared_ptr<BillBoard> billBoard = make_shared<BillBoard>();
			_selectedObject->AddComponent(billBoard);
		}

		bool renderer = (_selectedObject->GetMeshRenderer() == nullptr && 
			_selectedObject->GetModelRenderer() == nullptr &&
			_selectedObject->GetModelAnimator() == nullptr &&
			_selectedObject->GetSpriteRenderer() == nullptr);

		if (renderer && ImGui::Selectable("MeshRenderer"))
		{
			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			_selectedObject->AddComponent(meshRenderer);
		}

		if (renderer && ImGui::Selectable("ModelRenderer"))
		{
			shared_ptr<ModelRenderer> modelRenderer = make_shared<ModelRenderer>();
			_selectedObject->AddComponent(modelRenderer);
		}

		if (renderer && ImGui::Selectable("ModelAnimator"))
		{
			shared_ptr<ModelAnimator> modelAnimator = make_shared<ModelAnimator>();
			_selectedObject->AddComponent(modelAnimator);
		}

		if (renderer && ImGui::Selectable("SpriteRenderer"))
		{
			shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>();
			_selectedObject->AddComponent(spriteRenderer);
		}

		ImGui::TextDisabled("Script Components");
		ImGui::Separator();

		const auto& prototypeMap = GAME.GetLevelPrototype(_currentLevelIndex);
		for (const auto& pair :prototypeMap)
		{
			string key = Utils::ToString(pair.first);

			if (key.find("Script") != string::npos && ImGui::Selectable(key.c_str()))
			{
				shared_ptr<MonoBehaviour> script = dynamic_pointer_cast<MonoBehaviour>(GAME.Clone_Prototype(_currentLevelIndex, pair.first, nullptr));
				if(script != nullptr)
					_selectedObject->AddComponent(script);
			}
		}

		ImGui::EndPopup();
	}

	// ------------------------------------------- AddComponent -----------------------------------------------------

	for (const auto& fixedComponent : _selectedObject->GetComponents())
	{
		if (fixedComponent)
		{
			fixedComponent->OnInspectorGUI();
			//ImGui::Separator();
		}
	}
	for (const auto& script : _selectedObject->GetScripts())
	{
		if (script)
		{
			script->OnInspectorGUI();
			//ImGui::Separator();
		}
	}

	ImGui::End();
}

void Object_Manager::RenderGizmo()
{
	// 선택된 오브젝트가 없으면 그릴 필요 없음
	if (_selectedObject == nullptr)
		return;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	// --- ImGuizmo 프레임 시작 알림 ---
	ImGuizmo::BeginFrame();
	// 기즈모 설정
	ImGuizmo::SetOrthographic(false); // 원근 투영 사용 시 false


	// -----------------------ImGuizmo가 Default하게 생성하는 Debug창이 아니라 게임화면에 기즈모를 띄울 수 있게 설정-------
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	ImGuizmo::SetDrawlist(drawList);
	//----------------------------------------------------------------------------------------------------------------

	//ImVec2 viewPortPanelPos = ImGui::GetCursorScreenPos();
	//ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();
	ImGuizmo::SetRect(viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y);
	//ImGuizmo::SetRect(viewPortPanelPos.x, viewPortPanelPos.y, viewPortPanelSize.x, viewPortPanelSize.y);

	// 오브젝트의 World 행렬 가져오기
	auto pTransform = _selectedObject->GetTransform();
	Matrix worldMatrix = pTransform->GetWorldMatrix();

	if (!pTransform->HasParent())
	{
		if (ImGuizmo::Manipulate(
			(float*)&Camera::S_MatView,
			(float*)&Camera::S_MatProjection,
			_currentOp,
			ImGuizmo::WORLD,
			(float*)&worldMatrix))
		{
			pTransform->SetWorldMatrix(worldMatrix);
		}
	}
	else
	{
		if (ImGuizmo::Manipulate((float*)&Camera::S_MatView,
			(float*)&Camera::S_MatProjection, _currentOp, ImGuizmo::LOCAL, (float*)&worldMatrix))
		{
			pTransform->SetWorldMatrix(worldMatrix);
		}
	}
}

shared_ptr<Layer> Object_Manager::Find_Layer(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	if (iLayerLevelIndex >= _numLevels)
		return nullptr;

	auto iter = _layerMaps[iLayerLevelIndex].find(strLayerTag);
	if (iter == _layerMaps[iLayerLevelIndex].end())
		return nullptr;

	return iter->second;
}

unique_ptr<Object_Manager> Object_Manager::Create(uint32 iNumLevels)
{
	unique_ptr<Object_Manager> pInstance = unique_ptr<Object_Manager>(new Object_Manager);
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		return nullptr;
	}

	return pInstance;
}