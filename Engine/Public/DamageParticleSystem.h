#pragma once
#include "ParticleSystem.h"

NS_BEGIN(Engine)

class ENGINE_DLL DamageParticleSystem : public ParticleSystem
{
public:
	DamageParticleSystem();
	virtual ~DamageParticleSystem();

public:
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual HRESULT Render() override;

public:
	// 데미지 텍스트 전용 스폰 함수
	void SpawnDamageText(Vec3 startPos, int32 damageValue);
};

NS_END