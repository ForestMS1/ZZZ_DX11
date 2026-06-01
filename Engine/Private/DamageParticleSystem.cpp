#include "pch.h"
#include "DamageParticleSystem.h"
#include "Texture.h"
#include "Transform.h"
#include "Camera.h"
#include "InstancingBuffer.h"
DamageParticleSystem::DamageParticleSystem()
{
    _shader = GAME.GetResource<Shader>(L"DamageParticle.fx"); // 데미지 전용 셰이더 이름

	_texture = GAME.GetResource<Texture>(L"Skill_0N");

    // 셰이더의 배열 변수 이름과 바인딩 매칭
    _textureEffectBuffer = _shader->GetSRV("DamageTexArray");

	_fontColorTexture = GAME.GetResource<Texture>(L"DamageTextPhysDmg");
	__fontColorTextureEffectBuffer = _shader->GetSRV("ColorTex");
}

DamageParticleSystem::~DamageParticleSystem()
{
}

void DamageParticleSystem::Update()
{
	//ParticleSystem::Update();
    for (auto& particle : _particles)
    {
        particle.age += DT;
        if (particle.age >= particle.lifeTime) continue;

        if (particle.age <= particle.lifeTime * 0.5f)
            particle.position += particle.velocity * DT;

        if (particle.age >= particle.lifeTime * 0.75f)
            _alphaValue = (1 - (particle.lifeTime - particle.age));
    }
}

void DamageParticleSystem::FixedUpdate()
{
	ParticleSystem::FixedUpdate();
}

HRESULT DamageParticleSystem::Render()
{
    // 이번 프레임에 살아있는 파티클이 하나도 없으면 굳이 그릴 필요 없음
    if (_particles.empty())
        return S_OK;

    if (_mesh == nullptr || _shader == nullptr)
        return E_FAIL;

    _instancingBuffer->ClearData();

    Matrix billboardRot = Camera::S_MatView.Invert();
    billboardRot._41 = 0.f;
    billboardRot._42 = 0.f;
    billboardRot._43 = 0.f;

    uint32 aliveCount = 0;


    for (const auto& particle : _particles)
    {
        if (particle.age >= particle.lifeTime) continue;

        Matrix world = Matrix::CreateScale(particle.scale) * billboardRot;
        world._41 = particle.position.x;
        world._42 = particle.position.y;
        world._43 = particle.position.z;

        InstancingData data;
        data.world = world;
        data.etcInfo.x = particle.textureIndex;
        data.etcInfo.y = _alphaValue;
        //data.etcInfo.y = (1.f - (particle.age / particle.lifeTime));

        _instancingBuffer->AddData(data);
        aliveCount++;
    }

    if (aliveCount == 0)
        return S_OK;

    _instancingBuffer->PushData();

    _mesh->GetVertexBuffer()->PushData();
    _mesh->GetIndexBuffer()->PushData();

    if (_texture && _textureEffectBuffer)
        _textureEffectBuffer->SetResource(_texture->GetComPtr().Get());
    _shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);

    _shader->DrawIndexedInstanced(0, 0, _mesh->GetIndexBuffer()->GetCount(), aliveCount);

	if (_fontColorTexture && __fontColorTextureEffectBuffer)
		__fontColorTextureEffectBuffer->SetResource(_fontColorTexture->GetComPtr().Get());
	//return ParticleSystem::Render();
}

void DamageParticleSystem::SpawnDamageText(Vec3 startPos, Vec3 monsterScale, int32 damageValue, Vec3 minVelocity, Vec3 maxVelocity, float scale, float lifeTime)
{
    string damageStr = to_string(damageValue);

    std::uniform_real_distribution<float> offsetX(-monsterScale.x * 0.5f, monsterScale.x * 0.5f);
    std::uniform_real_distribution<float> offsetY(-monsterScale.y * 0.5f, monsterScale.y * 0.5f);
    std::uniform_real_distribution<float> offsetZ(-monsterScale.z * 0.5f, monsterScale.z * 0.5f);

    startPos += Vec3(offsetX(_gen), offsetY(_gen), offsetZ(_gen));

	// 글자가 생성될 때 중앙 정렬을 맞추기 위해 
	// 전체 글자 길이의 절반만큼 왼쪽으로 살짝 당겨서 시작

    float charOffset = 0.8f * scale;
    float startX = startPos.x - ((damageStr.size() - 1) * charOffset * 0.5f);

    // 난수 범위 설정 (X, Y, Z 각각 무작위 속도를 부여하기 위함)
    std::uniform_real_distribution<float> distX(minVelocity.x, maxVelocity.x);
    std::uniform_real_distribution<float> distY(minVelocity.y, maxVelocity.y);
    std::uniform_real_distribution<float> distZ(minVelocity.z, maxVelocity.z);

    Vec3 newVelocity(distX(_gen), distY(_gen), distZ(_gen));

    for (size_t i = 0; i < damageStr.size(); ++i)
    {
        int32 num = damageStr[i] - '0';

        ParticleInfo newParticle;

        newParticle.position = Vec3(startX + (i * charOffset), startPos.y, startPos.z);
		newParticle.scale = Vec3(scale);

        newParticle.velocity = newVelocity;
        newParticle.age = 0.f;
		newParticle.lifeTime = lifeTime;
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