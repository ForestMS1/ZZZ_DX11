#include "pch.h"
#include "TestPlayCamScript.h"
#include "TagManagerScript.h"

void TestPlayCamScript::Awake()
{
	_target = GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"Corin");

    auto camera = GetGameObject()->GetCamera();
    camera->LoadAction(L"NewAction");
    camera->LoadAction(L"CharacterChange");
    camera->LoadAction(L"Parry");

    // 이벤트함수 등록
    GAME.Subscribe(static_cast<uint32>(EventType::LEVEL_START), [this](const EventDesc& desc) { this->OnQuestStart(); });
    GAME.Subscribe(static_cast<uint32>(EventType::CHARACTER_SWITCH), [this](const EventDesc& desc) { this->OnCharacterSwitch(desc); });
    GAME.Subscribe(static_cast<uint32>(EventType::PARRY), [this](const EventDesc& desc) { this->OnParry(desc); });
}

void TestPlayCamScript::LateUpdate()
{
    if (_target.expired())
    {
        _target.reset();
        return;
    }

    auto camera = GetGameObject()->GetCamera();
    if (camera == nullptr || !camera->IsActive())
        return;

    // 1. 카메라 연출(Action)이 방금 막 종료된 순간 (자식 -> 월드 전환 시점)
    if (_isPrevActionPlay && !camera->IsPlaying())
    {
        Vec3 worldPos = GetTransform()->GetPosition();
        GetTransform()->SetParent(nullptr);
        GetTransform()->SetLocalPosition(worldPos);

        Vec3 lookDir = GetTransform()->GetLook();
        lookDir.Normalize();

        _curLookTarget = worldPos + lookDir * 5.0f;

        // atan2f의 기본 결과는 -PI ~ +PI 입니다.
        float rawYaw = atan2f(lookDir.x, lookDir.z);

        // ★ [추가] 음수로 나온 각도를 0 ~ 2*PI 대역의 양수 각도로 변환해 줍니다.
        if (rawYaw < 0.f)
            rawYaw += XM_2PI;

        _yaw = rawYaw; // 이제 마우스 범위와 완벽히 동기화됨!

        float length = sqrtf(lookDir.x * lookDir.x + lookDir.z * lookDir.z);
        _pitch = atan2f(-lookDir.y, length);
        _pitch = std::clamp(_pitch, -XM_PIDIV2 + 0.1f, XM_PIDIV2 - 0.1f);
    }

    // 2. 엔진 자체 카메라 보간 예외 처리
    if (GAME.IsInterpolating())
    {
        _isPrevActionPlay = camera->IsPlaying();
        return;
    }

    // 3. 윈도우 포커스 및 마우스 고정 처리 (기존과 동일)
    if (g_hWnd != GetFocus())
    {
        ShowCursor(true);
        return;
    }

    MouseFix();

    // 입력 값 누적 (연출이 끝났을 때만 마우스 조작 반영)
    if (!camera->IsPlaying())
    {
        float sensitivity = 0.1f;
        _yaw += GAME.Get_DIMouseMove(DIMS_X) * sensitivity * DT;
        _pitch += GAME.Get_DIMouseMove(DIMS_Y) * sensitivity * DT;
        _pitch = std::clamp(_pitch, -XM_PIDIV2 + 0.1f, XM_PIDIV2 - 0.1f);

        // _yaw 값이 한 바퀴를 넘어가면 0 ~ 2*PI 범위 안으로 계속 순환
        if (_yaw < 0.f) _yaw += XM_2PI;
        if (_yaw > XM_2PI) _yaw -= XM_2PI;
    }

    // 마우스 입력값 기반의 '이상적인 목표 월드 좌표(Ideal Target)' 계산
    Matrix matRot = Matrix::CreateFromYawPitchRoll(_yaw, _pitch, 0.f);
    float targetDistance = 5.0f;
    Vec3 offset = Vec3(0.f, 0.f, -targetDistance);
    offset = Vec3::TransformNormal(offset, matRot);

    Vec3 playerPos = _target.lock()->GetTransform()->GetPosition();
    Vec3 idealPos = playerPos + offset + Vec3(0.f, 1.5f, 0.f); // 마우스 조작 시 도달해야 할 최종 카메라 위치

    // 마우스 조작 모드에서 최종적으로 쳐다봐야 할 이상적인 타겟 지점 (플레이어 허리/머리)
    Vec3 idealLookTarget = playerPos + Vec3(0.f, 1.0f, 0.f);

    // 6. 상태에 따른 최종 카메라 배치 및 보간
    if (camera->IsPlaying())
    {
        _isPrevActionPlay = camera->IsPlaying();
        return;
    }
    else
    {
        // 연출이 끝나고 부모가 없는 상태: 위치와 '시선 지점(LookAt Target)'을 동시에 보간
        float blendSpeed = 6.0f;

        // 현재 카메라의 실제 위치에서 이상적인 위치로 보간
        Vec3 currentPos = GetTransform()->GetLocalPosition();
        Vec3 finalPos = Vec3::Lerp(currentPos, idealPos, blendSpeed * DT);

        // [핵심] 현재 카메라가 바라보는 시선 지점 자체를 플레이어 조작 시선 지점으로 보간 (Lerp)
        _curLookTarget = Vec3::Lerp(_curLookTarget, idealLookTarget, blendSpeed * DT);

        // 최종 적용: 계산된 위치로 옮기고, 보간 중인 시선 지점을 LookAt하게 만듦
        GetTransform()->SetLocalPosition(finalPos);
        GetTransform()->LookAtLocal(_curLookTarget);
    }

    _isPrevActionPlay = camera->IsPlaying();
}

void TestPlayCamScript::OnInspectorGUI()
{
    GuiRemoveButton("TestPlayCamScript");
    if (ImGui::CollapsingHeader("TestPlayCamScript", ImGuiTreeNodeFlags_DefaultOpen))
    {

    }
}

void TestPlayCamScript::OnQuestStart()
{
    auto target = _target.lock();
    target->GetTransform()->AddChild(GetTransform());
    const auto& cameraName = GetGameObject()->GetName();
    auto camera = GetGameObject()->GetCamera();
    GAME.ChangeCurCamera(cameraName);
    camera->Play(L"NewAction");
}

void TestPlayCamScript::OnCharacterSwitch(const EventDesc& desc)
{
    // 타겟 바꾸고 보간
    auto tagManager = reinterpret_cast<TagManagerScript*>(desc.sender);
    _target = tagManager->GetCurCharacter();
    //auto camera = GetGameObject()->GetCamera();
    //_target.lock()->GetTransform()->AddChild(GetTransform());
    //const auto& cameraName = GetGameObject()->GetName();
    //GAME.ChangeCurCamera(cameraName);
    //camera->Play(L"CharacterChange");
}

void TestPlayCamScript::OnParry(const EventDesc& desc)
{
    auto target = _target.lock();
    target->GetTransform()->AddChild(GetTransform());
    const auto& cameraName = GetGameObject()->GetName();
    auto camera = GetGameObject()->GetCamera();
    GAME.ChangeCurCamera(cameraName);
    camera->Play(L"Parry");
}

void TestPlayCamScript::MouseFix()
{
    // 디버그용
    if (GAME.Key_Pressing(DIK_Q))
        _mouseFix = false;
    else if (GAME.Key_Pressing(DIK_R))
        _mouseFix = true;

    if (_mouseFix)
    {
        POINT ptMouse{ GAME.GetEngineDesc().iWinSizeX >> 1, GAME.GetEngineDesc().iWinSizeY >> 1 };
        ClientToScreen(GAME.GetEngineDesc().hWnd, &ptMouse);
        SetCursorPos(ptMouse.x, ptMouse.y);
        ShowCursor(false);
    }
    else
    {
        ShowCursor(true);
    }
}

unique_ptr<TestPlayCamScript> TestPlayCamScript::Create()
{
    auto pInstance = unique_ptr<TestPlayCamScript>(new TestPlayCamScript);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : TestPlayCamScript");
        return nullptr;
    }

    return pInstance;
}
