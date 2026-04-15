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

	void SetSpeed(float speed) { _speed = speed; }
	float GetSpeed() const { return _speed; }
	// ImGUI
	virtual void OnInspectorGUI() override;

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
	uint32 _curTextureIndex = 0;
	// _textures Size
	uint32 _maxTextureNum = 0;
	// 다음 Texture로 넘어가는 스피드 값
	float _speed = 1.f;
	// 루프 여부
	bool _roop = false;
};

NS_END