#include "Global.fx"
#include "Light.fx"


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
    output.position.z = 0.00001f * output.position.w;
    
    output.uv = input.uv;
    
    output.normal = mul(input.normal, (float3x3) input.instanceWorld);
    output.tangent = mul(input.tangent, (float3x3) input.instanceWorld);
    
    output.texIndex = input.instanceEtcInfo.x;
    output.alpha = input.instanceEtcInfo.y;
    
    return output;

}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 output = DiffuseMap.Sample(LinearSampler, input.uv);
    
    // C++ 연산으로 계산된 투명도 멀티플라이
    output.a *= input.alpha;
    
    clip(output.a - 0.01f);
    clip(output.r + output.g + output.b - 0.1f);
    
    return output;
}


DepthStencilState DSS_DisableDepth
{
    DepthEnable = false; // 깊이 테스트 안 함 (무조건 덮어씀)
    DepthWriteMask = zero; // 깊이 버퍼에 기록도 안 함
};

technique11 T0
{
    pass P0
    {
        SetBlendState(BS_Alpha, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetDepthStencilState(DSS_DisableDepth, 0); // 깊이 버퍼 끄기
        
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}