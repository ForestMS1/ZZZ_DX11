#include "pch.h"
#include "ParticleSystem.h"
#include "InstancingBuffer.h"
#include "Camera.h"
#include "Texture.h"
#include "GameObject.h"
#include "DamageParticleSystem.h"

ParticleSystem::ParticleSystem()
	: Component(ComponentType::ParticleSystem)
{
	_shader = GAME.GetResource<Shader>(L"Particle.fx");
	_texture = GAME.GetResource<Texture>(L"IconRoleGeneral09");
	_textureEffectBuffer = _shader->GetSRV("DiffuseMap");
	_mesh = GAME.GetResource<Mesh>(L"Quad"); // 기본 DefaultMesh
	_instancingBuffer = make_shared<InstancingBuffer>();
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update()
{
	for (auto& particle : _particles)
	{
		particle.age += DT;
		if (particle.age >= particle.lifeTime) continue;

		particle.position += particle.velocity * DT;
		particle.scale += particle.scaleVelocity * DT;
	}
}

void ParticleSystem::FixedUpdate()
{
	bool anyAlive = false;
	for (auto& particle : _particles)
	{
		if (particle.age >= particle.lifeTime) continue;
		anyAlive = true; // 살아있는 알갱이가 하나라도 있음!
	}

	// 살아있는 파티클이 있다면
	if (anyAlive)
	{
		GAME.Add_RenderObject(RENDERGROUP::BLEND, GetGameObject());
	}
	else
	{
		// 모든 파티클이 죽었다면 이 게임오브젝트를 씬에서 삭제 처리를 예약
		SetLifeState(LIFESTATE::REMOVE);
		GetGameObject()->SetLifeState(LIFESTATE::REMOVE);
	}
}

HRESULT ParticleSystem::Render()
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

	return S_OK;
}

void ParticleSystem::CreateParticles(const Vec3& startPos, const Vec3& minVelocity, const Vec3& maxVelocity, const Vec3& scale, Vec3 scaleVelocity, float lifeTime, uint32 count)
{
	// 난수 범위 설정 (X, Y, Z 각각 무작위 속도를 부여하기 위함)
	std::uniform_real_distribution<float> distX(minVelocity.x, maxVelocity.x);
	std::uniform_real_distribution<float> distY(minVelocity.y, maxVelocity.y);
	std::uniform_real_distribution<float> distZ(minVelocity.z, maxVelocity.z);

	for (uint32 i = 0; i < count; ++i)
	{
		ParticleInfo newParticle;
		newParticle.position = startPos;
		newParticle.scale = scale;
		newParticle.scaleVelocity = scaleVelocity;
		newParticle.age = 0.0f;
		newParticle.lifeTime = lifeTime;

		// 사방으로 퍼지는 랜덤 속도 생성
		newParticle.velocity.x = distX(_gen);
		newParticle.velocity.y = distY(_gen);
		newParticle.velocity.z = distZ(_gen);

		// 메모리 재할당을 줄이기 위해 죽은 파티클 자리가 있다면 재활용하고, 없으면 push_back
		bool bReused = false;
		for (auto& particle : _particles)
		{
			if (particle.age >= particle.lifeTime) // 이미 수명이 다한 죽은 파티클 발견!
			{
				particle = newParticle; // 그 자리에 덮어쓰기
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

void ParticleSystem::OnInspectorGUI()
{
	GuiRemoveButton("ParticleSystem");
	if (ImGui::CollapsingHeader("ParticleSystem", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}
