#include "pch.h"
#include "NetworkView.h"
#include "INetworkObservable.h"
#include "NetworkManager.h"

HRESULT NetworkView::Initialize_Prototype()
{
    _observedComponents.fill(nullptr);
    return S_OK;
}

void NetworkView::Awake()
{
    _networkID = Utils::GUIDToUint32(GetGameObject()->GetId());
	NetworkManager::Get().AddNetworkView(SHARED_THIS(NetworkView));
}

void NetworkView::Update()
{
	if (_isMine) {
		// 내 캐릭터라면 일정 주기마다 상태 전송 (Throttling)
		auto now = std::chrono::steady_clock::now();
		if (now - _lastSendTime >= std::chrono::milliseconds(33)) { // 30fps
			SerializeState();
			_lastSendTime = now;
		}
	}
}

void NetworkView::OnInspectorGUI()
{
    // 컴포넌트 삭제 버튼 (기존 유틸리티 함수 사용)
    GuiRemoveButton("NetworkView");

    if (ImGui::CollapsingHeader("NetworkView", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 1. 네트워크 상태 정보 (읽기 전용)
        ImGui::Text("Network ID: %u", GetNetworkID());

        // IsMine 상태를 체크박스로 표시 (수동으로 바꿀 수도 있게 하거나, 혹은 상태만 표시)
        bool isMine = IsMine();
        if (ImGui::Checkbox("Is Mine", &isMine))
        {
            SetIsMine(isMine);
        }

        ImGui::Separator();

        // 2. 동기화 대상 컴포넌트 목록 관리
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Observed Components");

        if (_observedComponents.empty())
        {
            ImGui::TextDisabled("No components registered.");
        }
        else
        {
            for (int i = 0; i < _observedComponents.size(); ++i)
            {
                // 컴포넌트 이름이나 타입을 알 수 있다면 표시 (여기선 간단히 인덱스)
                ImGui::BulletText("Component [%d]", i);
            }
        }

        ImGui::Separator();

        // 3. 실시간 통신 통계 (디버깅용)
        ImGui::Text("Last Sequence: %u", _lastSequence);

        // Throttling 상태 확인
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastSendTime).count();
        ImGui::Text("Last Sent: %lld ms ago", diff);

        ImGui::Spacing();

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Force send current state to server regardless of throttling.");
    }
}

void NetworkView::SerializeState()
{
	// 실제 데이터(바디)를 담을 버퍼와 플래그 초기화
	std::vector<uint8_t> payload;
	uint32_t syncFlags = 0;

	// 등록된 컴포넌트들로부터 데이터 수집
	for (auto& obj : _observedComponents)
	{
        if (obj == nullptr)
            continue;

		obj->OnSerialize(payload, syncFlags);
        // 각 컴포넌트가 끝날 때마다 사이즈가 늘어나는지 확인
         printf("Component Serialized. Current Size: %llu\n", payload.size());
	}

	// 만약 보낼 데이터가 하나도 없다면 전송 중단
	if (syncFlags == 0) return;

	// 헤더 조립
	auto header = make_shared<GamePacket>();
	header->type = GameDataType::OBJECT_SYNC;
    header->objectId = _networkID;
	header->sequenceNumber = ++_lastSequence; // 매 전송마다 증가
	header->syncFlags = syncFlags;
	header->dataSize = static_cast<uint32_t>(payload.size());

	// 헤더와 바디를 묶어서 전송
	NetworkManager::Get().SendObjectSync(header, payload);
}

unique_ptr<NetworkView> NetworkView::Create()
{
	auto pInstance = unique_ptr<NetworkView>(new NetworkView);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : NetworkView");
		return nullptr;
	}

	return pInstance;
}