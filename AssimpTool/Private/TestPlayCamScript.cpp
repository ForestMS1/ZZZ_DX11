#include "pch.h"
#include "TestPlayCamScript.h"


void TestPlayCamScript::Awake()
{
	_target = GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"Corin");

    GetGameObject()->GetCamera()->LoadAction(L"NewAction");
    // 이벤트함수 등록
    GAME.Subscribe(static_cast<uint32>(EventType::LEVEL_START), [this](const EventDesc& desc) { this->OnQuestStart(); });
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

    if (_isPrevActionPlay && !camera->IsPlaying())
    {
        _pitch = 0.f;
        _yaw = 0.f;
    }

    if (g_hWnd != GetFocus())
    {
        ShowCursor(true);
        return;
    }

    if (GAME.Key_Down(DIK_Q))
        _mouseFix = !_mouseFix;

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

    // 입력 값 누적
    float sensitivity = 0.1f;
    _yaw += GAME.Get_DIMouseMove(DIMS_X) * sensitivity * DT;
    _pitch += GAME.Get_DIMouseMove(DIMS_Y) * sensitivity * DT;

    // Pitch 제한 .뒤집힘 방지
    _pitch = std::clamp(_pitch, -XM_PIDIV2 + 0.1f, XM_PIDIV2 - 0.1f);


    Matrix matRot = Matrix::CreateFromYawPitchRoll(_yaw, _pitch, 0.f);


    // 타겟으로부터 뒤쪽으로 일정 거리만큼 떨어진 벡터
    float distance = 5.0f;
    Vec3 offset = Vec3(0.f, 0.f, -distance);


    offset = Vec3::TransformNormal(offset, matRot);
    Vec3 targetPos = _target.lock()->GetTransform()->GetPosition();
    Vec3 finalPos = targetPos + offset + Vec3(0.f, 1.5f, 0.f);


    GetTransform()->SetLocalPosition(finalPos);
    GetTransform()->LookAt(targetPos + Vec3(0.f, 1.0f, 0.f)); // 플레이어의 허리/머리 쪽을 응시

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
    auto camera = GetGameObject()->GetCamera();
    camera->CameraOn();
    camera->Play(L"NewAction");
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
