#include "pch.h"
#include "LevelSaveLoader.h"
#include "GameObject.h"
#include <filesystem>
#include <fstream>
LevelSaveLoader::LevelSaveLoader()
{
}

LevelSaveLoader::~LevelSaveLoader()
{
}

void LevelSaveLoader::Save(uint32 iLevelIndex, const wstring& strLayerTag)
{
    filesystem::path savePath = filesystem::current_path().parent_path().parent_path() / L"Saved" / L"Levels";

    if (!filesystem::exists(savePath)) {
        filesystem::create_directories(savePath);
    }

    // 파일 이름 결정 (예: Level0_LayerDefault.bin)
    wstring fileName = L"Level_" + Utils::ToWString(GAME.GetEngineDesc().ppLevelNames[iLevelIndex]).append(L"_").append(strLayerTag).append(L".bin");
    filesystem::path filePath = savePath / fileName;

    // 파일 스트림 열기 (바이너리 쓰기 모드)
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) return;

    const list<shared_ptr<GameObject>>& gameObjectList = GAME.Get_GameObjects(iLevelIndex, strLayerTag);

    // 먼저 오브젝트 개수를 저장 (로드할 때 루프 횟수 결정용)
    uint32_t objCount = static_cast<uint32_t>(gameObjectList.size());
    outFile.write(reinterpret_cast<char*>(&objCount), sizeof(objCount));

    for (const auto& gameObject : gameObjectList)
    {
        GameObjectData data = {};
        // 고유 타입 설정
        const wstring& className = gameObject->Get_ClassName();
        const wstring& objectName = gameObject->GetName();

        data.objectId = gameObject->GetId();

        if (gameObject->GetTransform()->HasParent())
            data.parentId = gameObject->GetTransform()->GetParentTransform()->GetGameObject()->GetId();
        else
            UuidCreateNil(&data.parentId);

        // Transform 데이터 구조체 채우기
        data.transformData.localScale = gameObject->GetTransform()->GetLocalScale();
        data.transformData.localRotation = gameObject->GetTransform()->GetLocalRotation();
        data.transformData.localPosition = gameObject->GetTransform()->GetLocalPosition();
        data.transformData.matLocal = gameObject->GetTransform()->GetLocalMatrix();
        data.transformData.matWorld = gameObject->GetTransform()->GetWorldMatrix();

        // UI 오브젝트라면 UI 위치 저장
        auto spriteRenderer = gameObject->GetSpriteRenderer();
        if (spriteRenderer != nullptr && spriteRenderer->IsUI())
        {
            data.uiData.x = spriteRenderer->GetUIPosX();
            data.uiData.y = spriteRenderer->GetUIPosY();
            data.uiData.width = spriteRenderer->GetUIWidth();
            data.uiData.height = spriteRenderer->GetUIHeight();
        }

        auto collider = gameObject->GetCollider();
        if (collider != nullptr)
        {
            data.colliderData.isTrigger = collider->IsTrigger();
            data.colliderData.extents = collider->GetScale();
            data.colliderData.offset = collider->GetOffset();
        }



        // className 저장 (길이 -> 데이터)
        uint32_t classLen = static_cast<uint32_t>(className.size());
        outFile.write(reinterpret_cast<char*>(&classLen), sizeof(classLen));
        outFile.write(reinterpret_cast<const char*>(className.data()), classLen * sizeof(wchar_t));

        // objectName 저장 (길이 -> 데이터)
        uint32_t nameLen = static_cast<uint32_t>(objectName.size());
        outFile.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        outFile.write(reinterpret_cast<const char*>(objectName.data()), nameLen * sizeof(wchar_t));

        // [데이터 쓰기 시작]
        outFile.write(reinterpret_cast<char*>(&data.objectId), sizeof(UUID));
        outFile.write(reinterpret_cast<char*>(&data.parentId), sizeof(UUID));
        outFile.write(reinterpret_cast<char*>(&data.transformData), sizeof(TransformData));
        outFile.write(reinterpret_cast<char*>(&data.uiData), sizeof(UIData));
        outFile.write(reinterpret_cast<char*>(&data.colliderData), sizeof(ColliderData));
    }

    outFile.close();
}


struct UUIDComparer {
    bool operator()(const UUID& Left, const UUID& Right) const {
        return memcmp(&Left, &Right, sizeof(UUID)) < 0;
    }
};

void LevelSaveLoader::Load(uint32 iLevelIndex, const wstring& strLayerTag)
{
    // 경로 설정 (Save와 동일하게)
    filesystem::path loadPath = filesystem::current_path().parent_path().parent_path() / L"Saved" / L"Levels";
    wstring fileName = L"Level_" + Utils::ToWString(GAME.GetEngineDesc().ppLevelNames[iLevelIndex]).append(L"_").append(strLayerTag).append(L".bin");
    filesystem::path filePath = loadPath / fileName;

    if (!filesystem::exists(filePath)) return;

    // 바이너리 읽기 스트림
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) return;

    // 오브젝트 개수 읽기
    uint32_t objCount = 0;
    inFile.read(reinterpret_cast<char*>(&objCount), sizeof(objCount));

    // 로드 중간 단계 데이터를 보관할 구조체
    struct LoadedData {
        shared_ptr<GameObject> pObj;
        UUID parentId;
    };
    vector<LoadedData> loadedList;
    map<UUID, shared_ptr<GameObject>, UUIDComparer> objectMap; // UUID 비교 연산자 필요

    for (uint32_t i = 0; i < objCount; ++i)
    {
        // className 읽기
        uint32_t classLen = 0;
        inFile.read(reinterpret_cast<char*>(&classLen), sizeof(classLen));
        wstring className;
        className.resize(classLen);
        inFile.read(reinterpret_cast<char*>(className.data()), classLen * sizeof(wchar_t));

        // objectName 읽기
        uint32_t nameLen = 0;
        inFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        wstring objectName;
        objectName.resize(nameLen);
        inFile.read(reinterpret_cast<char*>(objectName.data()), nameLen * sizeof(wchar_t));

        // 고정 데이터(ID, Transform) 읽기
        UUID objId, parentId;
        TransformData tData;
        UIData uData;
        ColliderData colData;
        inFile.read(reinterpret_cast<char*>(&objId), sizeof(UUID));
        inFile.read(reinterpret_cast<char*>(&parentId), sizeof(UUID));
        inFile.read(reinterpret_cast<char*>(&tData), sizeof(TransformData));
        inFile.read(reinterpret_cast<char*>(&uData), sizeof(UIData));
        inFile.read(reinterpret_cast<char*>(&colData), sizeof(ColliderData));

        // 객체 생성 Factory 패턴
        // className에 따라 실제 클라이언트 객체를 생성해야 함
        shared_ptr<GameObject> newObj = GAME.CreateFromFactory(className);
        if (!newObj) continue;

        // 데이터 복원
        newObj->SetName(objectName);
        newObj->Set_ClassName(className);
        newObj->SetId(objId); // 세이브된 ID 유지

        auto transform = newObj->GetTransform();
        transform->SetLocalScale(tData.localScale);
        transform->SetLocalRotation(tData.localRotation);
        transform->SetLocalPosition(tData.localPosition);

        auto spriteRenderer = newObj->GetSpriteRenderer();
        if (spriteRenderer != nullptr)
        {
            spriteRenderer->SetUI(true);
            spriteRenderer->SetUIPosX(uData.x);
            spriteRenderer->SetUIPosY(uData.y);
            spriteRenderer->SetUIWidth(uData.width);
            spriteRenderer->SetUIHeight(uData.height);
        }

        auto collider = newObj->GetCollider();
        if (collider != nullptr)
        {
            collider->SetTrigger(colData.isTrigger);
            collider->SetScale(colData.extents);
            collider->SetOffset(colData.offset);
        }

        // 리스트에 추가 (레이어 등록)
        GAME.Add_GameObject_toLayerNoClone(iLevelIndex, strLayerTag, newObj);

        // 관계 복원을 위해 맵에 기록
        objectMap[objId] = newObj;
        loadedList.push_back({ newObj, parentId });
    }

    // Pass 2: 부모-자식 관계 연결 AddChild 
    for (auto& item : loadedList)
    {
        RPC_STATUS status;
        // 부모 ID가 Nil이 아니라면
        if (UuidIsNil(&item.parentId, &status) == FALSE)
        {
            // 내 부모를 맵에서 찾는다
            auto it = objectMap.find(item.parentId);
            if (it != objectMap.end())
            {
                // 부모 객체(it->second)가 나(item.pObj)를 자식으로 추가하게 함
                shared_ptr<GameObject> parentObj = it->second;
                parentObj->GetTransform()->AddChild(item.pObj->GetTransform());
            }
        }
    }

    inFile.close();
}