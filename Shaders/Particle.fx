#include "Global.fx"
#include "Light.fx"

float g_AlphaValue = 1.f;


struct VS_INPUT
{
    // Slot 0 (Per-Vertex Data)
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;

    // Slot 1 (Per-Instance Data)
    matrix instanceWorld : INSTANCE_WORLD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = mul(input.position, input.instanceWorld);
    output.position = mul(output.position, VP);
    
    output.uv = input.uv;
    
    output.normal = mul(input.normal, (float3x3) input.instanceWorld);
    output.tangent = mul(input.tangent, (float3x3) input.instanceWorld);
    
    return output;

}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 output = DiffuseMap.Sample(LinearSampler, input.uv);
    
    clip(output.a - 0.3f);
    
    output.a = output.a * g_AlphaValue;
    
    return output;
}

technique11 T0
{
    PASS_BS_VP(P0, BS_Alpha, VS, PS)
}