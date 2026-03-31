#pragma once

#include "Pass.h"
#include "Technique.h"

NS_BEGIN(Engine)

struct ShaderDesc
{
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DX11Effect> effect;
};

class ENGINE_DLL Shader
{
public:
	Shader(wstring file);
	~Shader();

	wstring GetFile() { return _file; }
	ComPtr<ID3DX11Effect> Effect() { return _shaderDesc.effect; }

	void Draw(UINT technique, UINT pass, UINT vertexCount, UINT startVertexLocation = 0);
	void DrawIndexed(UINT technique, UINT pass, UINT indexCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0);
	void DrawInstanced(UINT technique, UINT pass, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation = 0, UINT startInstanceLocation = 0);
	void DrawIndexedInstanced(UINT technique, UINT pass, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0, UINT startInstanceLocation = 0);

	void Dispatch(UINT technique, UINT pass, UINT x, UINT y, UINT z);

	ComPtr<ID3DX11EffectVariable> GetVariable(const string& name);
	ComPtr<ID3DX11EffectScalarVariable> GetScalar(const string& name);
	ComPtr<ID3DX11EffectVectorVariable> GetVector(const string& name);
	ComPtr<ID3DX11EffectMatrixVariable> GetMatrix(const string& name);
	ComPtr<ID3DX11EffectStringVariable> GetString(const string& name);
	ComPtr<ID3DX11EffectShaderResourceVariable> GetSRV(const string& name);
	ComPtr<ID3DX11EffectRenderTargetViewVariable> GetRTV(const string& name);
	ComPtr<ID3DX11EffectDepthStencilViewVariable> GetDSV(const string& name);
	ComPtr<ID3DX11EffectUnorderedAccessViewVariable> GetUAV(const string& name);
	ComPtr<ID3DX11EffectConstantBuffer> GetConstantBuffer(const string& name);
	ComPtr<ID3DX11EffectShaderVariable> GetShader(const string& name);
	ComPtr<ID3DX11EffectBlendVariable> GetBlend(const string& name);
	ComPtr<ID3DX11EffectDepthStencilVariable> GetDepthStencil(const string& name);
	ComPtr<ID3DX11EffectRasterizerVariable> GetRasterizer(const string& name);
	ComPtr<ID3DX11EffectSamplerVariable> GetSampler(const string& name);

private:
	void CreateEffect();
	ComPtr<ID3D11InputLayout> CreateInputLayout(ComPtr<ID3DBlob> fxBlob, D3DX11_EFFECT_SHADER_DESC* effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& params);


private:
	wstring _file;
	ShaderDesc _shaderDesc;
	D3DX11_EFFECT_DESC _effectDesc;
	shared_ptr<StateBlock> _initialStateBlock;
	vector<Technique> _techniques;
};

class ENGINE_DLL ShaderManager
{
public:
	static ShaderDesc GetEffect(const wstring& fileName);

private:
	static unordered_map<wstring, ShaderDesc> shaders;
};

NS_END
