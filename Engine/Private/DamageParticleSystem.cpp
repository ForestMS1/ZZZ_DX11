#include "pch.h"
#include "DamageParticleSystem.h"
#include "Texture.h"

DamageParticleSystem::DamageParticleSystem()
{
    _shader = GAME.GetResource<Shader>(L"DamageParticle.fx"); // 데미지 전용 셰이더 이름

	_texture = GAME.GetResource<Texture>(L"Skill_0N");

    // 셰이더의 배열 변수 이름과 바인딩 매칭
    _textureEffectBuffer = _shader->GetSRV("DamageTexArray");
}

DamageParticleSystem::~DamageParticleSystem()
{
}

void DamageParticleSystem::Update()
{
	ParticleSystem::Update();
}

void DamageParticleSystem::FixedUpdate()
{
	ParticleSystem::FixedUpdate();
}

HRESULT DamageParticleSystem::Render()
{
	return ParticleSystem::Render();
}

void DamageParticleSystem::SpawnDamageText(Vec3 startPos, int32 damageValue)
{
	string damageStr = to_string(damageValue);

	// 글자가 생성될 때 중앙 정렬을 맞추기 위해 
	// 전체 글자 길이의 절반만큼 왼쪽으로 살짝 당겨서 시작
	float charOffset = 0.4f;
	float startX = startPos.x - ((damageStr.size() - 1) * charOffset * 0.5f);

	for (size_t i = 0; i < damageStr.size(); ++i)
	{
		int32 num = damageStr[i] - '0';

		ParticleInfo newParticle;

		newParticle.position = Vec3(startX + (i * charOffset), startPos.y, startPos.z);
		newParticle.scale = Vec3(0.5f, 0.5f, 0.5f);


		newParticle.velocity = Vec3(0.f, 2.5f, 0.f);
		newParticle.age = 0.f;
		newParticle.lifeTime = 0.8f;
		newParticle.textureIndex = static_cast<float>(num);

		// 죽은 자리가 있다면 덮어쓰기 (메모리 재활용)
		bool bReused = false;
		for (auto& particle : _particles)
		{
			if (particle.age >= particle.lifeTime)
			{
				particle = newParticle;
				bReused = true;
				break;
			}
		}

		if (!bReused)
		{
			_particles.push_back(newParticle);
		}
	}
}