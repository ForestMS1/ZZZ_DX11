#include "Global.fx"

Texture2D sprite;

MeshOutput VS(VertexTextureNormalTangentBlend input)
{
    MeshOutput output;
    output.position = mul(input.position, W);
    output.worldPosition = input.position.xyz;
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) W);
    
    return output;
}

float4 PS(MeshOutput input) : SV_TARGET
{
    return sprite.Sample(LinearSampler, input.uv);
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
};