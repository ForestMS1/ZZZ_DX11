#pragma once
#include "Component.h"
#include <random>

NS_BEGIN(Engine)

struct ParticleInfo
{
	Vec3 position;
	Vec3 scale;
	Vec3 velocity;
	Vec3 scaleVelocity;
	float age = 0.f;
	float lifeTime = 1.f;
	float textureIndex = 0.f;
	float localXOffset = 0.f;
};

class InstancingBuffer;

class ENGINE_DLL ParticleSystem : public Component
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

public:
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual HRESULT Render() override;

	void SetShader(shared_ptr<Shader> shader) { _shader = shader; _textureEffectBuffer = _shader->GetSRV("DiffuseMap"); }
	void SetTexture(shared_ptr<Texture> texture) { _texture = texture; }

	void CreateParticles(const Vec3& startPos, const Vec3& minVelocity, const Vec3& maxVelocity, const Vec3& scale, Vec3 scaleVelocity, float lifeTime, uint32 count);

	virtual void OnInspectorGUI() override;
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }
protected:
	vector<ParticleInfo> _particles;
	shared_ptr<InstancingBuffer> _instancingBuffer;

	shared_ptr<Shader> _shader;
	shared_ptr<Mesh> _mesh;
	shared_ptr<Texture> _texture;
	ComPtr<ID3DX11EffectShaderResourceVariable> _textureEffectBuffer;

	RENDERGROUP _renderGroup = RENDERGROUP::BLEND; // 따로 설정 안했으면 BLEND

	// 무작위 속도를 주기 위한 난수 생성용 헬퍼 변수 (선택)
	std::random_device _rd;
	std::mt19937 _gen{ _rd() };
};

NS_END