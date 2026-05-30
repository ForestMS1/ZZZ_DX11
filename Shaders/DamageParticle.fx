#include "Global.fx"
#include "Light.fx"

// 일반 Texture2D 대신 Texture2DArray 변수를 선언
Texture2DArray DamageTexArray;

struct VS_INPUT
{
    // Slot 0 (Per-Vertex Data)
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;

    // Slot 1 (Per-Instance Data)
    matrix instanceWorld : INSTANCE_WORLD;
    float4 instanceEtcInfo : INSTANCE_ETC;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    
    float1 texIndex : TEXINDEX;
    float1 alpha : ALPHA;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = mul(input.position, input.instanceWorld);
    output.position = mul(output.position, VP);
    
    output.uv = input.uv;
    
    output.normal = mul(input.normal, (float3x3) input.instanceWorld);
    output.tangent = mul(input.tangent, (float3x3) input.instanceWorld);
    
    output.texIndex = input.instanceEtcInfo.x;
    output.alpha = input.instanceEtcInfo.y;
    
    return output;

}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    
    // Texture2DArray를 샘플링할 때는 UV 좌표로 float3(U, V, SliceIndex)를 넘겨야
    float3 uvw = float3(input.uv, input.texIndex);
    float4 color = DamageTexArray.Sample(LinearSampler, uvw);
    
    // C++ 연산으로 계산된 투명도 멀티플라이
    color.a *= input.alpha;
    
    clip(color.a - 0.01f);
    
    return color;
}

technique11 T0
{
    PASS_BS_VP(P0, BS_Alpha, VS, PS)
}