#include "pch.h"
#include "CollisionManager.h"
#include "GameObject.h"
#include "Layer.h"
#include "MonoBehaviour.h"
#include "AABBCollider.h"

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

HRESULT CollisionManager::Initialize(uint32 iNumLevels)
{
	_collisionLayerTable.resize(iNumLevels);
	return S_OK;
}

void CollisionManager::FixedUpdate()
{
	// 이번 프레임에 충돌 중인 쌍들을 모을 임시 컨테이너
	set<CollisionPair> currentPairs;

	for (auto& pair : _collisionLayerTable[GAME.GetCurrentLevelIndex()])
	{
		shared_ptr<Layer> srcLayer = GAME.Find_CurrentLevel_Layer(pair.first);
		shared_ptr<Layer> dscLayer = GAME.Find_CurrentLevel_Layer(pair.second);
		if (srcLayer == nullptr || dscLayer == nullptr)
			return;

		for (auto& srcGameObject : srcLayer->Get_GameObjects())
		{
			// 한 프레임도 안돈 애들 충돌처리 x (바운딩 박스 위치, 크기 안정해짐)
			if (LIFESTATE::STARTED != srcGameObject->GetLifeState())
				continue;

			if (shared_ptr<Collider> srcCollider = srcGameObject->GetCollider())
			{
				for (auto& dscGameObject : dscLayer->Get_GameObjects())
				{
					// 한 프레임도 안돈 애들 충돌처리 x (바운딩 박스 위치, 크기 안정해짐)
					if (LIFESTATE::STARTED != dscGameObject->GetLifeState())
						continue;
					// 자기 자신과 충돌처리 방지
					if (srcGameObject == dscGameObject)
						continue;
					// 부모/자식 관계 충돌처리 방지
					if ((srcGameObject->GetTransform()->GetParentTransform() == dscGameObject->GetTransform()) || 
						(dscGameObject->GetTransform()->GetParentTransform() == srcGameObject->GetTransform()))
						continue;

					if (shared_ptr<Collider> dscCollider = dscGameObject->GetCollider())
					{
						//충돌 검사.
						if (srcCollider->Intersects(dscCollider))
						{
							CollisionPair cp(srcCollider, dscCollider); // 정렬된 쌍 생성
							currentPairs.insert(cp);

							Collision collision{ srcCollider, dscCollider };

							// AABB끼리고 둘다 Trigger false면 밀어냄, 둘중 하나라도 Trigger true면 겹친 값만 Collision에 저장
							Collision_RectEx(srcCollider, dscCollider, &collision);
							if(srcLayer != dscLayer) // 같은 레이어면 어차피 두번 됨
								Collision_RectEx(dscCollider, srcCollider, nullptr);

							// Enter
							if (_previousPairs.find(cp) == _previousPairs.end())
							{
								// 충돌한 오브젝트들의 MonoBehaviour 스크립트에 뿌려준다.
								for (shared_ptr<MonoBehaviour> script : srcGameObject->GetScripts())
								{
									script->OnCollisionEnter(collision);
								}
								for (shared_ptr<MonoBehaviour> script : dscGameObject->GetScripts())
								{
									script->OnCollisionEnter(collision);
								}
							}
							// Stay
							else
							{
								// 충돌한 오브젝트들의 MonoBehaviour 스크립트에 뿌려준다.
								for (shared_ptr<MonoBehaviour> script : srcGameObject->GetScripts())
								{
									script->OnCollisionStay(collision);
								}
								for (shared_ptr<MonoBehaviour> script : dscGameObject->GetScripts())
								{
									script->OnCollisionStay(collision);
								}
							}
						}
					}
				}
			}
		}
	}

	for (const auto& prevPair : _previousPairs)
	{
		if (currentPairs.find(prevPair) == currentPairs.end())
		{
			// 객체 및 콜라이더 유효성 체크
			if (prevPair.colliderA.expired() || prevPair.colliderB.expired())
				continue;

			auto objA = prevPair.colliderA.lock()->GetGameObject();
			auto objB = prevPair.colliderB.lock()->GetGameObject();

			// 오브젝트가 삭제 대기 상태(REMOVE)인지 확인
			if (objA == nullptr || objB == nullptr)
				continue;

			if (objA->GetLifeState() == LIFESTATE::REMOVE || objB->GetLifeState() == LIFESTATE::REMOVE)
				continue;

			Collision exitCollision{ prevPair.colliderA, prevPair.colliderB, 0.f, 0.f, 0.f };
			for (shared_ptr<MonoBehaviour> script : prevPair.colliderA.lock()->GetGameObject()->GetScripts())
			{
				script->OnCollisionExit(exitCollision);
			}
			for (shared_ptr<MonoBehaviour> script : prevPair.colliderB.lock()->GetGameObject()->GetScripts())
			{
				script->OnCollisionExit(exitCollision);
			}
		}
	}

	_previousPairs = std::move(currentPairs);
}

void CollisionManager::AddCollisionLayer(uint32 iLayerLevelIndex, const wstring& strLayerTagA, const wstring& strLayerTagB)
{
	if (iLayerLevelIndex >= _collisionLayerTable.size())
		return;

	_collisionLayerTable[iLayerLevelIndex].push_back(make_pair(strLayerTagA, strLayerTagB));
}

void CollisionManager::ClearPair()
{
	_previousPairs.erase(_previousPairs.begin(), _previousPairs.end());
}

unique_ptr<CollisionManager> CollisionManager::Create(uint32 iNumLevels)
{
	unique_ptr<CollisionManager> pInstance = unique_ptr<CollisionManager>(new CollisionManager);
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CollisionManager");
		return nullptr;
	}

	return pInstance;
}

void CollisionManager::Collision_RectEx(shared_ptr<Collider> pSrcAABB, shared_ptr<Collider> pDstAABB, Collision* pInfo)
{
	auto src = dynamic_pointer_cast<AABBCollider>(pSrcAABB);
	auto dsc = dynamic_pointer_cast<AABBCollider>(pDstAABB);
	if (src == nullptr || dsc == nullptr)
		return;
	if (src->IsTrigger() || dsc->IsTrigger())
		return;

	auto& boxA = src->GetBoundingBox();
	auto& boxB = dsc->GetBoundingBox();

	// 공식: (A반지름 + B반지름) - |A중심 - B중심|
	float overlapX = (boxA.Extents.x + boxB.Extents.x) - fabsf(boxA.Center.x - boxB.Center.x);
	float overlapY = (boxA.Extents.y + boxB.Extents.y) - fabsf(boxA.Center.y - boxB.Center.y);
	float overlapZ = (boxA.Extents.z + boxB.Extents.z) - fabsf(boxA.Center.z - boxB.Center.z);

	// Collision 구조체에 값 저장
	if (pInfo != nullptr)
	{
		pInfo->overlapX = overlapX;
		pInfo->overlapY = overlapY;
		pInfo->overlapZ = overlapZ;
	}

	if (pSrcAABB->IsFix())
		return;

	auto transform = pSrcAABB->GetTransform();
	Vec3 pos = transform->GetLocalPosition();

	// X축 겹침이 가장 작을 때
	if (overlapX < overlapY && overlapX < overlapZ)
	{
		// src가 dsc보다 오른쪽에 있으면 +방향, 왼쪽에 있으면 -방향으로 밀기
		float dir = (boxA.Center.x > boxB.Center.x) ? 1.0f : -1.0f;
		pos.x += overlapX * dir;
	}
	// Y축 겹침이 가장 작을 때
	else if (overlapY < overlapX && overlapY < overlapZ)
	{
		float dir = (boxA.Center.y > boxB.Center.y) ? 1.0f : -1.0f;
		pos.y += overlapY * dir;
	}
	// Z축 겹침이 가장 작을 때
	else
	{
		float dir = (boxA.Center.z > boxB.Center.z) ? 1.0f : -1.0f;
		pos.z += overlapZ * dir;
	}

	transform->SetLocalPosition(pos);
}