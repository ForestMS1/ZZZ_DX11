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

        particle.position += particle.velocity * DT;
    }
}

void DamageParticleSystem::FixedUpdate()
{
	ParticleSystem::FixedUpdate();
}

HRESULT DamageParticleSystem::Render()
{
    if (_particles.empty()) return S_OK;
    if (_mesh == nullptr || _shader == nullptr) return E_FAIL;

    _instancingBuffer->ClearData();

    Matrix billboardRot = Camera::S_MatView.Invert();
    billboardRot._41 = 0.f;
    billboardRot._42 = 0.f;
    billboardRot._43 = 0.f;

    // 빌보드 기준 카메라의 우측 방향 벡터(World Right)를 추출
    Vec3 billboardRight = Vec3(billboardRot._11, billboardRot._12, billboardRot._13);

    uint32 aliveCount = 0;

    for (const auto& particle : _particles)
    {
        if (particle.age >= particle.lifeTime) continue;

        Matrix world = Matrix::CreateScale(particle.scale) * billboardRot;

        // 전체 텍스트 중심의 현재 월드 위치 계산 (위치 + 속도*시간)
        // Update에서 하던 위치 이동을 감안한 최종 중심점
        Vec3 centerPos = particle.position;

        // 빌보드의 Right 벡터를 기준으로 개별 글자의 오프셋을 적용
        // 이렇게 하면 카메라가 돌아가도 글자의 가로 정렬 방향이 항상 카메라 우측을 향함
        Vec3 finalPos = centerPos + (billboardRight * particle.localXOffset);

        world._41 = finalPos.x;
        world._42 = finalPos.y;
        world._43 = finalPos.z;

        InstancingData data;
        data.world = world;
        data.etcInfo.x = particle.textureIndex;
        data.etcInfo.y = (1.f - (particle.age / particle.lifeTime));

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
}

void DamageParticleSystem::SpawnDamageText(Vec3 startPos, Vec3 monsterScale, int32 damageValue, Vec3 minVelocity, Vec3 maxVelocity, float scale, float lifeTime)
{
    string damageStr = to_string(damageValue);

    std::uniform_real_distribution<float> offsetX(-monsterScale.x * 0.5f, monsterScale.x * 0.5f);
    std::uniform_real_distribution<float> offsetY(-monsterScale.y * 0.5f, monsterScale.y * 0.5f);
    std::uniform_real_distribution<float> offsetZ(-monsterScale.z * 0.5f, monsterScale.z * 0.5f);

    std::uniform_real_distribution<float> randScale(scale * 0.5f, scale * 1.1f);
    scale = randScale(_gen);

    // 텍스트 전체가 생성될 중심 월드 좌표
    startPos += Vec3(offsetX(_gen), offsetY(_gen), offsetZ(_gen));

    float charOffset = 0.8f * scale;
    // 이제 startX는 월드 좌표가 아니라 로컬 오프셋의 시작점입니다.
    float startX = -((damageStr.size() - 1) * charOffset * 0.5f);

    std::uniform_real_distribution<float> distX(minVelocity.x, maxVelocity.x);
    std::uniform_real_distribution<float> distY(minVelocity.y, maxVelocity.y);
    std::uniform_real_distribution<float> distZ(minVelocity.z, maxVelocity.z);

    Vec3 newVelocity(distX(_gen), distY(_gen), distZ(_gen));

    for (size_t i = 0; i < damageStr.size(); ++i)
    {
        int32 num = damageStr[i] - '0';

        ParticleInfo newParticle;

        // position에는 기준이 되는 중심 월드 좌표
        newParticle.position = startPos;
        newParticle.scale = Vec3(scale);
        newVelocity = newVelocity;
        newParticle.age = 0.f;
        newParticle.lifeTime = lifeTime;
        newParticle.textureIndex = static_cast<float>(num);

        newParticle.localXOffset = startX + (i * charOffset);


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
        if (!bReused) _particles.push_back(newParticle);
    }
}