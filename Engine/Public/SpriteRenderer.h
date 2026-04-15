#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class Texture;

class ENGINE_DLL SpriteRenderer : public Component
{
public:
	SpriteRenderer(shared_ptr<Shader> shader);
	~SpriteRenderer();
public:

	// 생명주기 함수
	virtual void Update() override;
	virtual HRESULT Render() override;

	void Add_Texture(shared_ptr<Texture> texture) { if (texture == nullptr)return; _textures.push_back(texture); }

	// Mesh Set
	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }

	// Shader Property
	shared_ptr<Shader> GetShader() { return _shader; }
	void SetShader(shared_ptr<Shader> shader) { _shader = shader; _textureEffectBuffer = _shader->GetSRV("DiffuseMap"); }

	// Speed Property
	void SetSpeed(float speed) { _speed = speed; }
	float GetSpeed() const { return _speed; }
	
	// isPlay Property
	bool IsPlay() { return _play; }
	void SetPlay(bool play) { _play = play; }

	// Loop ProPerty
	bool IsLoop() { return _loop; }
	void SetLoop(bool loop) { _loop = loop; }

	// ImGUI
	virtual void OnInspectorGUI() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) { return nullptr; }

private:
	shared_ptr<Shader> _shader;
	shared_ptr<Mesh> _mesh;
	uint8 _techniqueIndex = 0;
	uint8 _pass = 0;
	RENDERGROUP _renderGroup = RENDERGROUP::PRIORITY; // 따로 설정 안했으면 PRIORITY
	ComPtr<ID3DX11EffectShaderResourceVariable> _textureEffectBuffer;

	// 애니메이션으로 보여줄 Texture들
	vector<shared_ptr<Texture>> _textures;
	// 현재 보여줄 Texture Index
	float _curTextureIndex = 0;
	// 다음 Texture로 넘어가는 스피드 값
	float _speed = 1.f;
	// 루프 여부
	bool _loop = false;
	// 재생 여부
	bool _play = true;
};

NS_END