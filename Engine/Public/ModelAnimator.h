#pragma once
#include "Component.h"

NS_BEGIN(Engine)

struct AnimTransform
{
	// [ ][ ][ ][ ][ ][ ][ ] ... 350°³
	using TransformArrayType = array<Matrix, MAX_MODEL_TRANSFORMS>;
	// [ ][ ][ ][ ][ ][ ][ ] ... 500 °³
	array<TransformArrayType, MAX_MODEL_KEYFRAMES> transforms;
};

class Shader;
class Model;


class ENGINE_DLL ModelAnimator : public Component
{
public:
	ModelAnimator(shared_ptr<Shader> shader);
	~ModelAnimator();

	virtual HRESULT Render() override;

	void SetModel(shared_ptr<Model> model);
	void SetPass(uint8 pass) { _pass = pass; }

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }

private:
	void CreateTexture();
	void CreateAnimationTransform(uint32 index);

private:
	vector<AnimTransform> _animTransforms;
	ComPtr<ID3D11Texture2D> _texture;
	ComPtr<ID3D11ShaderResourceView> _srv;

private:
	KeyframeDesc _keyframeDesc;
	TweenDesc _tweenDesc;

private:
	shared_ptr<Shader>	_shader;
	uint8				_pass = 0;
	shared_ptr<Model>	_model;
};

NS_END