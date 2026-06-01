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
	void SpawnDamageText(Vec3 startPos, Vec3 monsterScale, int32 damageValue, Vec3 minVelocity = { -1.f, -1.f, -1.f }, Vec3 maxVelocity = { 1.f, 1.f, 1.f }, float scale = 0.25f, float lifeTime = 1.5f);

private:
	shared_ptr<Texture> _fontColorTexture;
	ComPtr<ID3DX11EffectShaderResourceVariable> __fontColorTextureEffectBuffer;
};

NS_END