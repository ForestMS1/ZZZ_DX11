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

					if (shared_ptr<Collider> dscCollider = dscGameObject->GetCollider())
					{
						// 충돌체 구조체 만들고
						Collision collision;
						collision.colliderA = srcCollider;
						collision.colliderB = dscCollider;

						CollisionPair collisionPair;
						collisionPair.colliderA = srcCollider;
						collisionPair.colliderB = dscCollider;

						//충돌 검사.
						if (srcCollider->Intersects(dscCollider))
						{
							// AABB끼리고 둘다 Trigger false면 밀어냄, 둘중 하나라도 Trigger true면 겹친 값만 Collision에 저장
							Collision_RectEx(srcCollider, dscCollider, &collision);

							// Enter
							if (_currentPairs.find(collisionPair) == _currentPairs.end())
							{
								// 충돌쌍 삽입
								_currentPairs.insert(collisionPair);
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
							if (_currentPairs.find(collisionPair) != _currentPairs.end())
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
						// 둘이 이번프레임에 충돌안했음
						else
						{
							//Exit
							if (_currentPairs.find(collisionPair) != _currentPairs.end())
							{
								// 충돌 쌍 제거
								_currentPairs.erase(collisionPair);
								// 충돌한 오브젝트들의 MonoBehaviour 스크립트에 뿌려준다.
								for (shared_ptr<MonoBehaviour> script : srcGameObject->GetScripts())
								{
									script->OnCollisionExit(collision);
								}
								for (shared_ptr<MonoBehaviour> script : dscGameObject->GetScripts())
								{
									script->OnCollisionExit(collision);
								}
							}
						}
					}
				}
			}
		}
	}
}

void CollisionManager::AddCollisionLayer(uint32 iLayerLevelIndex, const wstring& strLayerTagA, const wstring& strLayerTagB)
{
	if (iLayerLevelIndex >= _collisionLayerTable.size())
		return;

	_collisionLayerTable[iLayerLevelIndex].emplace(strLayerTagA, strLayerTagB);
}

void CollisionManager::ClearPair()
{
	_currentPairs.erase(_currentPairs.begin(), _currentPairs.end());
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
	pInfo->overlapX = overlapX;
	pInfo->overlapY = overlapY;
	pInfo->overlapZ = overlapZ;


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