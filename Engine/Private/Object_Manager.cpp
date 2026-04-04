#include "pch.h"
#include "Object_Manager.h"
#include "Layer.h"

#include "GameInstance.h"
#include "GameObject.h"

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

HRESULT Object_Manager::Add_GameObject_toLayer(uint32 iPrototypeLevelIndex, const wstring& strPrototypeTag, uint32 iLayerLevelIndex, const wstring& strLayerTag, void* pArg)
{
 	if (nullptr == _layerMaps || iLayerLevelIndex >= _numLevels)
		return E_FAIL;

	shared_ptr<GameObject> pGameObject = dynamic_pointer_cast<GameObject>(GameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag));
	if (nullptr == pGameObject)
		return E_FAIL;

	// 현재 객체를 추가하려고 하는 레이어가 없다면
	// 새로 레이어를 만들어 추가해준다.
	Layer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
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

void Object_Manager::ShowHiearchy()
{
	ImGui::Begin("Scene Hierarchy");

	ImGui::Text("Current Level: %d", _currentLevelIndex);
	ImGui::Separator();

	auto& currentLayerMap = _layerMaps[_currentLevelIndex];

	for (auto& pair : currentLayerMap) // unique_ptr 내부 맵 접근
	{
		const wstring& layerTag = pair.first;
		Layer* pLayer = pair.second.get();

		string tagStr = Utils::ToString(layerTag);
		ImGuiTreeNodeFlags layerFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

		if (ImGui::TreeNodeEx(tagStr.c_str(), layerFlags))
		{
			const list<shared_ptr<GameObject>>& objects = pLayer->Get_GameObjects();

			for (auto& pGameObject : objects)
			{
				string objName = Utils::ToString(pGameObject->GetName());

				// 선택된 상태라면 하이라이트 효과 적용
				ImGuiTreeNodeFlags objFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
				if (_selectedObject == pGameObject)
					objFlags |= ImGuiTreeNodeFlags_Selected;

				// 고유 ID로 포인터 주소 사용
				ImGui::TreeNodeEx((void*)pGameObject.get(), objFlags, objName.c_str());

				// 좌클릭 시 선택
				if (ImGui::IsItemClicked())
				{
					_selectedObject = pGameObject;
				}
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void Object_Manager::ShowInspector()
{
	ImGui::Begin("Inspector");

	if (_selectedObject == nullptr)
	{
		ImGui::TextColored(ImVec4(1, 1, 1, 0.5f), "Select an object to view details.");
		ImGui::End();
		return;
	}

	// --- 기본 정보 (이름 등) ---
	char buf[256];
	string currentName = Utils::ToString(_selectedObject->GetName());
	strcpy_s(buf, currentName.c_str());

	if (ImGui::InputText("Name", buf, 256))
	{
		_selectedObject->SetName(Utils::ToWString(buf)); // 수정 시 즉시 반영
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
		Vec3 pos = pTransform->GetPosition();
		// DragFloat3의 너비를 조절하여 버튼 공간 확보
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		if (ImGui::DragFloat3("Position", (float*)&pos, 0.1f))
		{
			pTransform->SetPosition(pos);
		}
		ImGui::PopItemWidth();

		ImGui::SameLine(); // 같은 줄에 배치
		if (ImGui::Button("R##ResetPos")) // ## 뒤의 이름은 ID로 작동하여 중복 방지
		{
			pTransform->SetPosition(Vec3(0.f, 0.f, 0.f));
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Position");

		// --- Rotation ---
		Vec3 rot = pTransform->GetRotation();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		if (ImGui::DragFloat3("Rotation", (float*)&rot, 0.5f))
		{
			pTransform->SetRotation(rot);
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button("R##ResetRot"))
		{
			pTransform->SetRotation(Vec3(0.f, 0.f, 0.f));
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Rotation");

		// --- Scale ---
		Vec3 scale = pTransform->GetScale();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f))
		{
			pTransform->SetScale(scale);
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button("R##ResetScale"))
		{
			pTransform->SetScale(Vec3(1.f, 1.f, 1.f));
		}
	}

	ImGui::Separator();

	auto pMeshRenderer = _selectedObject->GetMeshRenderer();
	if(pMeshRenderer)
		pMeshRenderer->OnInspectorGUI();

	ImGui::Separator();

	auto pModelRenderer = _selectedObject->GetModelRenderer();
	if (pModelRenderer)
		pModelRenderer->OnInspectorGUI();

	auto pModelAnimator = _selectedObject->GetModelAnimator();
	if (pModelAnimator)
		pModelAnimator->OnInspectorGUI();

	// 기타 다른 컴포넌트들도 유사한 방식으로 추가...
	// for (auto& component : _selectedObject->GetComponents()) { ... }

	ImGui::End();
}

void Object_Manager::RenderGizmo()
{
	// 1. 선택된 오브젝트가 없으면 그릴 필요 없음
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

	ImGuizmo::SetRect(viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y);

	// 오브젝트의 World 행렬 가져오기
	auto pTransform = _selectedObject->GetTransform();
	Matrix worldMatrix = pTransform->GetWorldMatrix();

	// 6. 조작 결과 계산
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



Layer* Object_Manager::Find_Layer(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	if (iLayerLevelIndex >= _numLevels)
		return nullptr;

	auto iter = _layerMaps[iLayerLevelIndex].find(strLayerTag);
	if (iter == _layerMaps[iLayerLevelIndex].end())
		return nullptr;

	return iter->second.get();
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