#include "Global.fx"

Texture2D sprite;

MeshOutput VS(VertexTextureNormalTangent input)
{
    MeshOutput output;
    output.position = mul(input.position, W);
    output.worldPosition = input.position.xyz;
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) W);
    
    return output;
}

float4 PS(MeshOutput input)
{
    return sprite.Sample(LinearSampler, input.uv);
}