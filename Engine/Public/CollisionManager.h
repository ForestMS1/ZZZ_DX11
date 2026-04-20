#pragma once

NS_BEGIN(Engine)

class Collider;

struct CollisionPair
{
	shared_ptr<Collider> colliderA;
	shared_ptr<Collider> colliderB;

	bool operator< (const CollisionPair& rhs) const
	{
		return colliderA < rhs.colliderA;
	}
};

class CollisionManager
{
private:
	CollisionManager();
public:
	~CollisionManager();

	HRESULT Initialize(uint32 iNumLevels);

	void	FixedUpdate();

	// 충돌 레이어 추가
	void	AddCollisionLayer(uint32 iLayerLevelIndex, const wstring& strLayerTagA, const wstring& strLayerTagB);
	
	void	Collision_RectEx(shared_ptr<Collider> pSrcAABB, shared_ptr<Collider> pDstAABB, struct Collision* pInfo);

	// 씬이 바뀌었을때 충돌 쌍 정보 날리기
	void	ClearPair();

private:
	// 충돌처리 레이어 테이블 (인덱스는 레벨의미)
	vector<map<const wstring, const wstring>> _collisionLayerTable;

	// 충돌 쌍 정보 (이벤트 함수 호출 타이밍을 관리하기 위해)
	std::set<CollisionPair> _currentPairs;

public:
	static unique_ptr<CollisionManager> Create(uint32 iNumLevels);
};

NS_END

