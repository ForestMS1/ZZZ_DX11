#include "Global.fx"

struct VS_IN
{
    float4 position : POSITION;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 clipPos : TEXCOORD0; // 깊이 비교를 위해 전달
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    
    // W : 물체의 월드 행렬
    // VP : 광원(Light)의 View-Projection 행렬 (C++에서 넘겨줘야 함)
    output.position = mul(input.position, W);
    output.position = mul(output.position, VP);
    output.clipPos = output.position;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // 깊이 값(Z/W)을 R 채널에 기록 (0~1 사이 값)
    float depth = input.clipPos.z / input.clipPos.w;
    return float4(depth, depth, depth, 1.0f);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetPixelShader(CompileShader(ps_5_0, PS_Main()));
    }
};