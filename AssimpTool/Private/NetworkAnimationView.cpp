#include "pch.h"
#include "NetworkAnimationView.h"
#include "NetworkView.h"

NetworkAnimationView::NetworkAnimationView()
	: INetworkObservable(NetworkViewType::AnimationView)
{
}

NetworkAnimationView::~NetworkAnimationView()
{
}

void NetworkAnimationView::Awake()
{
	_view = GetGameObject()->GetScript<NetworkView>();
	if (!_view.expired())
		_view.lock()->RegisterView(this);
	_modelAnimatorCom = GetGameObject()->GetModelAnimator();
}

void NetworkAnimationView::Update()
{
	if (_view.expired())
	{
		_view.reset();
		return;
	}
	if (_modelAnimatorCom.expired())
	{
		_modelAnimatorCom.reset();
		return;
	}
}

void NetworkAnimationView::OnSerialize(std::vector<uint8_t>& outPayload, uint32_t& outFlags)
{
	auto animator = GetGameObject()->GetModelAnimator();
	if (!animator) return;

	const TweenDesc& desc = animator->GetTweenDesc();

	// 기본 애니메이션 데이터 채우기
	AnimationData animData;
	animData.animIndex = desc.curr.animIndex;
	animData.currFrame = desc.curr.currFrame;

	auto fsm = animator->GetFSM();
	if (!fsm) return;

	auto& boolMap = fsm->GetBoolMap();
	auto& floatMap = fsm->GetFloatMap();
	auto& triggerMap = fsm->GetTriggerMap();

	// FSM의 모든 파라미터 수집 (예: _floatParams 맵 전체)
	// wstring 키를 FNV-1a 해시로 변환해서 담습니다.
	std::vector<AnimParamData> allParams;
	for (auto& [name, value] : floatMap) {
		allParams.push_back({ Utils::Hash(name), value });
	}
	for (auto& [name, value] : boolMap) {
		allParams.push_back({ Utils::Hash(name), value ? 1.0f : 0.0f });
	}
	for (auto& [name, value] : triggerMap) {
		allParams.push_back({ Utils::Hash(name), value ? 1.0f : 0.0f });
	}

	animData.paramCount = static_cast<uint16_t>(allParams.size());

	// 페이로드 구성
	size_t offset = outPayload.size();
	size_t totalSize = sizeof(AnimationData) + (sizeof(AnimParamData) * animData.paramCount);
	outPayload.resize(offset + totalSize);

	memcpy(outPayload.data() + offset, &animData, sizeof(AnimationData));
	if (animData.paramCount > 0) {
		memcpy(outPayload.data() + offset + sizeof(AnimationData),
			allParams.data(), sizeof(AnimParamData) * animData.paramCount);
	}

	outFlags |= SyncFlag::ANIMATION;
}

void NetworkAnimationView::OnDeserialize(const void* data)
{
	// 주권 확인 (내가 조종하는 캐릭터라면 서버 데이터를 무시함)
	auto view = _view.lock();
	if (view && view->IsMine())
		return;

	const AnimationData* animData = static_cast<const AnimationData*>(data);
	auto animator = GetGameObject()->GetModelAnimator();
	if (!animator) return;

	auto fsm = animator->GetFSM();
	if (!fsm) return;

	const AnimParamData* paramBase = reinterpret_cast<const AnimParamData*>(animData + 1);
	for (int i = 0; i < animData->paramCount; ++i)
	{
		const AnimParamData& p = paramBase[i];
		// 해시값을 이용해 내 FSM 파라미터 업데이트 (이름 복원 과정 필요 없음)
		fsm->UpdateParamByHash(p.paramHash, p.value);
	}
}

void NetworkAnimationView::OnDestroy()
{
	if (!_view.expired())
		_view.lock()->RemoveView(_viewType);
}

void NetworkAnimationView::OnInspectorGUI()
{
	// 컴포넌트 삭제 버튼
	GuiRemoveButton("NetworkAnimationView");

	if (ImGui::CollapsingHeader("NetworkAnimationView", ImGuiTreeNodeFlags_DefaultOpen))
	{
		
	}
}

unique_ptr<NetworkAnimationView> NetworkAnimationView::Create()
{
	auto pInstance = unique_ptr<NetworkAnimationView>(new NetworkAnimationView);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : NetworkAnimationView");
		return nullptr;
	}

	return pInstance;
}