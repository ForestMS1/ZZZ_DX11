#include "Global.fx"
#include "Light.fx"

Texture2D RenderTargetUI;

VertexTextureNormalTangent VS(VertexTextureNormalTangent input)
{
    VertexTextureNormalTangent output;
    
    output.position = mul(input.position, W);
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) W);
    output.tangent = mul(input.tangent, (float3x3) W);
    
    return output;

}

VertexTexture VS_RENDERTARGET_UI(VertexTexture input)
{
    VertexTexture output;
    

    output.position = mul(input.position, W);
    output.position = mul(output.position, VP);
    output.uv = input.uv;

    return output;
}

float4 PS(VertexTextureNormalTangent input) : SV_TARGET
{
    return DiffuseMap.Sample(LinearSampler, input.uv);
}

float4 PS_RENDERTARGET_UI(VertexTexture input) : SV_TARGET
{
    return RenderTargetUI.Sample(LinearSampler, input.uv);
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
    PASS_VP(P1, VS_RENDERTARGET_UI, PS_RENDERTARGET_UI)
};