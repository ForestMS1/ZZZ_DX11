#include "Global.fx"
#include "Light.fx"


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

float4 PS(VertexTextureNormalTangent input) : SV_TARGET
{
    return DiffuseMap.Sample(LinearSampler, input.uv);
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
};