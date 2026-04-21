#include "pch.h"
#include "LevelSaveLoader.h"
#include "GameObject.h"
#include "Texture.h"
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
        shared_ptr<newGameObjectData> data = make_shared<newGameObjectData>();
        // 고유 타입 설정
        const wstring& className = gameObject->Get_ClassName();
        const wstring& objectName = gameObject->GetName();

        data->objectId = gameObject->GetId();

        if (gameObject->GetTransform()->HasParent())
            data->parentId = gameObject->GetTransform()->GetParentTransform()->GetGameObject()->GetId();
        else
            UuidCreateNil(&data->parentId);

        // Transform 데이터 구조체 채우기
        data->transformData.localScale = gameObject->GetTransform()->GetLocalScale();
        data->transformData.localRotation = gameObject->GetTransform()->GetLocalRotation();
        data->transformData.localPosition = gameObject->GetTransform()->GetLocalPosition();
        data->transformData.matLocal = gameObject->GetTransform()->GetLocalMatrix();
        data->transformData.matWorld = gameObject->GetTransform()->GetWorldMatrix();

        // UI 오브젝트라면 UI 위치 저장
        auto spriteRenderer = gameObject->GetSpriteRenderer();
        if (spriteRenderer != nullptr && spriteRenderer->IsUI())
        {
            data->uiData.isSave = true;
            data->uiData.x = spriteRenderer->GetUIPosX();
            data->uiData.y = spriteRenderer->GetUIPosY();
            data->uiData.width = spriteRenderer->GetUIWidth();
            data->uiData.height = spriteRenderer->GetUIHeight();
            data->uiData.textureCount = spriteRenderer->GetTextureCount();
            
            // 쉐이더 이름 저장
            auto shader = spriteRenderer->GetShader();
            if (shader != nullptr)
            {
                size_t copyLen = std::min((int)shader->GetName().size(), 63); // 마지막 널 문자를 위해 63자 제한
                wcsncpy_s(data->uiData.shaderName, shader->GetName().c_str(), copyLen);
                data->uiData.shaderName[copyLen] = L'\0';
            }

            auto& textures = spriteRenderer->GetTextures();
            for (uint32 i = 0; i < textures.size(); ++i)
            {
                auto& tex = textures[i];

                size_t copyLen = std::min((int)tex->GetName().size(), 127); // 마지막 널 문자를 위해 63자 제한
                wcsncpy_s(data->uiData.textureNames[i], tex->GetName().c_str(), copyLen);
                data->uiData.textureNames[i][copyLen] = L'\0';
            }

            data->uiData.speed = spriteRenderer->GetSpeed();
            data->uiData.loop = spriteRenderer->IsLoop();
            data->uiData.play = spriteRenderer->IsPlay();
        }

        // 콜라이더 저장
        auto collider = gameObject->GetCollider();
        if (collider != nullptr)
        {
            data->colliderData.isSave = true;
            data->colliderData.type = collider->GetColliderType();
            data->colliderData.isTrigger = collider->IsTrigger();
            data->colliderData.isFix = collider->IsFix();
            data->colliderData.extents = collider->GetScale();
            data->colliderData.offset = collider->GetOffset();
        }

        // 모델렌더러 저장
        auto modelRenderer = gameObject->GetModelRenderer();
        if (modelRenderer != nullptr)
        {
            data->modelRenderData.isSave = true;
            // 쉐이더 이름 저장
            auto shader = modelRenderer->GetShader();
            if (shader != nullptr)
            {
                size_t copyLen = std::min((int)shader->GetName().size(), 63); // 마지막 널 문자를 위해 63자 제한
                wcsncpy_s(data->modelRenderData.shaderName, shader->GetName().c_str(), copyLen);
                data->modelRenderData.shaderName[copyLen] = L'\0';
            }
            // 모델 이름 저장
            auto model = modelRenderer->GetModel();
            if(model != nullptr)
            {
                size_t copyLen = std::min((int)model->GetName().size(), 63); // 마지막 널 문자를 위해 63자 제한
                wcsncpy_s(data->modelRenderData.modelName, model->GetName().c_str(), copyLen);
                data->modelRenderData.modelName[copyLen] = L'\0';
            }
            data->modelRenderData.techniqueIndex = modelRenderer->GetTechnique();
            data->modelRenderData.pass = modelRenderer->GetPass();
        }

        // 모델애니메이터 저장
        auto modelAnimator = gameObject->GetModelAnimator();
        if (modelAnimator != nullptr)
        {
            data->modelAnimData.isSave = true;
            // 쉐이더 이름 저장
            auto shader = modelAnimator->GetShader();
            if (shader != nullptr)
            {
                size_t copyLen = std::min((int)shader->GetName().size(), 63); // 마지막 널 문자를 위해 63자 제한
                wcsncpy_s(data->modelAnimData.shaderName, shader->GetName().c_str(), copyLen);
                data->modelAnimData.shaderName[copyLen] = L'\0';
            }
            // 모델 이름 저장
            auto model = modelAnimator->GetModel();
            if (model != nullptr)
            {
                size_t copyLen = std::min((int)model->GetName().size(), 63); // 마지막 널 문자를 위해 63자 제한
                wcsncpy_s(data->modelAnimData.modelName, model->GetName().c_str(), copyLen);
                data->modelAnimData.modelName[copyLen] = L'\0';
            }
            // 애니메이션 fsm 파일 이름 저장
            auto fsm = modelAnimator->GetFSM();
            if (fsm != nullptr)
            {
                size_t copyLen = std::min((int)fsm->GetFileName().size(), 63); // 마지막 널 문자를 위해 63자 제한
                wcsncpy_s(data->modelAnimData.animFSMFileName, fsm->GetFileName().c_str(), copyLen);
                data->modelAnimData.animFSMFileName[copyLen] = L'\0';
            }
            data->modelAnimData.techniqueIndex = modelAnimator->GetTechnique();
            data->modelAnimData.pass = modelAnimator->GetPass();
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
        outFile.write(reinterpret_cast<char*>(&data->objectId), sizeof(UUID));
        outFile.write(reinterpret_cast<char*>(&data->parentId), sizeof(UUID));
        outFile.write(reinterpret_cast<char*>(&data->transformData), sizeof(TransformData));
        outFile.write(reinterpret_cast<char*>(&data->uiData), sizeof(UIData));
        outFile.write(reinterpret_cast<char*>(&data->colliderData), sizeof(ColliderData));
        outFile.write(reinterpret_cast<char*>(&data->modelRenderData), sizeof(ModelRenderData));
        outFile.write(reinterpret_cast<char*>(&data->modelAnimData), sizeof(ModelAnimData));
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
        shared_ptr<TransformData> tData = make_shared<TransformData>();
        shared_ptr<UIData> uData = make_shared<UIData>();
        shared_ptr<ColliderData> colData = make_shared<ColliderData>();
        shared_ptr<ModelRenderData> modelRenderData = make_shared<ModelRenderData>();
        shared_ptr<ModelAnimData> modelAnimData = make_shared<ModelAnimData>();

        inFile.read(reinterpret_cast<char*>(&objId), sizeof(UUID));
        inFile.read(reinterpret_cast<char*>(&parentId), sizeof(UUID));
        inFile.read(reinterpret_cast<char*>(tData.get()), sizeof(TransformData));
        inFile.read(reinterpret_cast<char*>(uData.get()), sizeof(UIData));
        inFile.read(reinterpret_cast<char*>(colData.get()), sizeof(ColliderData));
        inFile.read(reinterpret_cast<char*>(modelRenderData.get()), sizeof(ModelRenderData));
        inFile.read(reinterpret_cast<char*>(modelAnimData.get()), sizeof(ModelAnimData));

        // 객체 생성 Factory 패턴
        // className에 따라 실제 클라이언트 객체를 생성해야 함
        shared_ptr<GameObject> newObj = GAME.CreateFromFactory(className);
        if (!newObj) continue;

        // 데이터 복원
        newObj->SetName(objectName);
        newObj->Set_ClassName(className);
        newObj->SetId(objId); // 세이브된 ID 유지

        auto transform = newObj->GetTransform();
        transform->SetLocalScale(tData->localScale);
        transform->SetLocalRotation(tData->localRotation);
        transform->SetLocalPosition(tData->localPosition);

        auto spriteRenderer = newObj->GetSpriteRenderer();
        if (uData->isSave == true && spriteRenderer == nullptr) // 데이터 저장은 했는데 해당 컴포넌트가 오브젝트에 없다면
        {
            auto newSpriteRenderer = make_shared<SpriteRenderer>(GAME.GetResource<Shader>(uData->shaderName));
            newSpriteRenderer->SetUI(true);
            newSpriteRenderer->SetUIPosX(uData->x);
            newSpriteRenderer->SetUIPosY(uData->y);
            newSpriteRenderer->SetUIWidth(uData->width);
            newSpriteRenderer->SetUIHeight(uData->height);

            // 텍스쳐 셋팅
            for (uint8 i = 0; i < uData->textureCount; ++i)
            {
                newSpriteRenderer->Add_Texture(GAME.GetResource<Texture>(uData->textureNames[i]));
            }
            newSpriteRenderer->SetSpeed(uData->speed);
            newSpriteRenderer->SetLoop(uData->loop);
            newSpriteRenderer->SetPlay(uData->play);
            newObj->AddComponent(newSpriteRenderer);
        }
        else if (spriteRenderer != nullptr)
        {
            spriteRenderer->SetShader(GAME.GetResource<Shader>(uData->shaderName));
            spriteRenderer->SetUI(true);
            spriteRenderer->SetUIPosX(uData->x);
            spriteRenderer->SetUIPosY(uData->y);
            spriteRenderer->SetUIWidth(uData->width);
            spriteRenderer->SetUIHeight(uData->height);

            spriteRenderer->GetTextures().clear();
            // 텍스쳐 셋팅
            for (uint8 i = 0; i < uData->textureCount; ++i)
            {
                spriteRenderer->Add_Texture(GAME.GetResource<Texture>(uData->textureNames[i]));
            }
            spriteRenderer->SetSpeed(uData->speed);
            spriteRenderer->SetLoop(uData->loop);
            spriteRenderer->SetPlay(uData->play);
        }

        auto collider = newObj->GetCollider();
        if (colData->isSave == true && collider == nullptr) // 데이터 저장은 했는데 해당 컴포넌트가 오브젝트에 없다면
        {
            shared_ptr<Collider> newCollider;
            switch (colData->type)
            {
            case ColliderType::AABB:
                newCollider = make_shared<AABBCollider>();
                break;
            case ColliderType::OBB:
                newCollider = make_shared<OBBCollider>();
                break;
            case ColliderType::Sphere:
                newCollider = make_shared<SphereCollider>();
                break;
            default:
                break;
            }
            newCollider->SetTrigger(colData->isTrigger);
            newCollider->SetFix(colData->isFix);
            newCollider->SetScale(colData->extents);
            newCollider->SetOffset(colData->offset);

            newObj->AddComponent(newCollider);
        }
        else if (collider != nullptr)
        {
            collider->SetTrigger(colData->isTrigger);
            collider->SetFix(colData->isFix);
            collider->SetScale(colData->extents);
            collider->SetOffset(colData->offset);
        }

        auto modelRenderer = newObj->GetModelRenderer();
        if (modelRenderData->isSave == true && modelRenderer == nullptr) // 데이터 저장은 했는데 해당 컴포넌트가 오브젝트에 없다면
        {
            shared_ptr<ModelRenderer> newModelRenderer = make_shared<ModelRenderer>(GAME.GetResource<Shader>(modelRenderData->shaderName));
          
            newModelRenderer->SetModel(GAME.GetResource<Model>(modelRenderData->modelName));
            newModelRenderer->SetTechnique(modelRenderData->techniqueIndex);
            newModelRenderer->SetPass(modelRenderData->pass);

            newObj->AddComponent(newModelRenderer);
        }
        else if (modelRenderer != nullptr)
        {
            modelRenderer->SetModel(GAME.GetResource<Model>(modelRenderData->modelName));
            modelRenderer->SetTechnique(modelRenderData->techniqueIndex);
            modelRenderer->SetPass(modelRenderData->pass);
        }

        auto modelAnimator = newObj->GetModelAnimator();
        if (modelAnimData->isSave == true && modelAnimator == nullptr) // 데이터 저장은 했는데 해당 컴포넌트가 오브젝트에 없다면
        {
            shared_ptr<ModelAnimator> newModelAnimator = make_shared<ModelAnimator>(GAME.GetResource<Shader>(modelAnimData->shaderName));

            newModelAnimator->SetModel(GAME.GetResource<Model>(modelAnimData->modelName));
            if (modelAnimData->animFSMFileName)
            {
                shared_ptr<AnimFSM> fsm = make_shared<AnimFSM>(newModelAnimator);
                fsm->Load(Utils::ToString(modelAnimData->animFSMFileName), newModelAnimator);
            }
            newModelAnimator->SetTechnique(modelAnimData->techniqueIndex);
            newModelAnimator->SetPass(modelAnimData->pass);

            newObj->AddComponent(newModelAnimator);
        }
        else if (modelAnimator != nullptr)
        {
            modelAnimator->SetModel(GAME.GetResource<Model>(modelAnimData->modelName));
            if (modelAnimData->animFSMFileName)
            {
                shared_ptr<AnimFSM> fsm = make_shared<AnimFSM>(modelAnimator);
                fsm->Load(Utils::ToString(modelAnimData->animFSMFileName), modelAnimator);
            }
            modelAnimator->SetTechnique(modelAnimData->techniqueIndex);
            modelAnimator->SetPass(modelAnimData->pass);
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