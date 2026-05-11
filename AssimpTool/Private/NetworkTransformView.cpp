#include "pch.h"
#include "NetworkTransformView.h"
#include "NetworkView.h"

NetworkTransformView::NetworkTransformView()
    : INetworkObservable(NetworkViewType::TransfomrView)
{
}

NetworkTransformView::~NetworkTransformView()
{
}

void NetworkTransformView::Awake()
{
	_view = GetGameObject()->GetScript<NetworkView>();
	if (!_view.expired())
		_view.lock()->RegisterView(this);
	_transformCom = GetGameObject()->GetTransform();
}

void NetworkTransformView::Update()
{
	if (_view.expired())
	{
		_view.reset();
		return;
	}
	if (_transformCom.expired())
	{
		_transformCom.reset();
		return;
	}
}

void NetworkTransformView::OnSerialize(std::vector<uint8_t>& outPayload, uint32_t& outFlags)
{
	// TODO : 위치가 변했을 때만 보내는 로직을 추가
	PlayerPosition data;
    Vec3 pos = GetTransform()->GetPosition();
    Vec3 rot = GetTransform()->GetRotation();
    Vec3 scale = GetTransform()->GetScale();
    data.posX = pos.x;
    data.posY = pos.y;
    data.posZ = pos.z;
    data.rotX = rot.x;
    data.rotY = rot.y;
    data.rotZ = rot.z;
    data.scaleX = scale.x;
    data.scaleY = scale.y;
    data.scaleZ = scale.z;

	data.timestamp = static_cast<uint32_t>(
		std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count());

	// 버퍼 끝에 데이터 추가
	size_t offset = outPayload.size();
	outPayload.resize(offset + sizeof(PlayerPosition));
	memcpy(outPayload.data() + offset, &data, sizeof(PlayerPosition));

	// 나 보냈다고 깃발 올리기
	outFlags |= SyncFlag::POSITION;
}

void NetworkTransformView::OnDeserialize(const void* data)
{
    // 1. 주권 확인 (내가 조종하는 캐릭터라면 서버 데이터를 무시함)
    auto view = _view.lock();
    if (view && view->IsMine())
        return;

    // 2. 데이터 캐스팅
    const PlayerPosition* receivedData = reinterpret_cast<const PlayerPosition*>(data);

    auto transform = _transformCom.lock();
    if (!transform) return;

    // 3. 데이터 반영 
    // [Tip] 즉시 반영하면 뚝뚝 끊길 수 있으므로, 
    // 나중에는 변수(m_targetPos)에 저장 후 Update에서 Lerp 처리를 권장합니다.
    transform->SetPosition(Vec3(receivedData->posX, receivedData->posY, receivedData->posZ));
    transform->SetRotation(Vec3(receivedData->rotX, receivedData->rotY, receivedData->rotZ));
    transform->SetScale(Vec3(receivedData->scaleX, receivedData->scaleY, receivedData->scaleZ));

    // 4. (선택 사항) 타임스탬프 확인 로그
    // uint32_t currentTick = ... (현재 시간);
    // uint32_t latency = currentTick - receivedData->timestamp;
    // std::cout << "Latency: " << latency << "ms" << std::endl;
}

void NetworkTransformView::OnDestroy()
{
    if (!_view.expired())
        _view.lock()->RemoveView(_viewType);
}

void NetworkTransformView::OnInspectorGUI()
{
    // 컴포넌트 삭제 버튼
    GuiRemoveButton("NetworkTransformView");

    if (ImGui::CollapsingHeader("NetworkTransformView", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto transform = _transformCom.lock();
        if (!transform)
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Transform Component not found!");
            return;
        }

        // 현재 실시간 좌표 정보 출력 (Read Only)
        ImGui::Text("Current State (Local)");
        Vec3 pos = transform->GetPosition();
        Vec3 rot = transform->GetRotation();

        ImGui::BulletText("Pos: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
        ImGui::BulletText("Rot: (%.2f, %.2f, %.2f)", rot.x, rot.y, rot.z);

        ImGui::Separator();

        // 동기화 옵션 설정
        // 이 옵션들은 나중에 OnSerialize에서 syncFlags를 결정할 때 사용하면 좋습니다.
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Sync Settings");

        static bool syncPos = true;
        static bool syncRot = true;
        static bool syncScale = false;

        ImGui::Checkbox("Sync Position", &syncPos);
        ImGui::Checkbox("Sync Rotation", &syncRot);
        ImGui::Checkbox("Sync Scale", &syncScale);

        ImGui::Separator();

        // 네트워크 뷰 연결 상태 확인
        auto view = _view.lock();
        if (view)
        {
            ImGui::Text("Linked NetworkView ID: %u", view->GetNetworkID());
            if (view->IsMine())
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: Sending (Owner)");
            else
                ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "Status: Receiving (Proxy)");
        }
        else
        {
            ImGui::TextDisabled("Status: Not linked to NetworkView");
        }

        ImGui::Spacing();
    }
}

unique_ptr<NetworkTransformView> NetworkTransformView::Create()
{
	auto pInstance = unique_ptr<NetworkTransformView>(new NetworkTransformView);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : NetworkTransformView");
		return nullptr;
	}

	return pInstance;
}