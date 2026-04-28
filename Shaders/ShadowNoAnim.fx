#include "Global.fx"
#include "Light.fx"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 clipPos : TEXCOORD0; // 깊이 비교를 위해 전달
};

struct PS_OUT
{
    float4 shadowDepth : SV_Target0;
};

VS_OUT VS_Main(VertexTextureNormalTangentBlend input)
{
    VS_OUT output;
    
    // W : 물체의 월드 행렬
    // VP : 광원(Light)의 View-Projection 행렬 (C++에서 넘겨줘야 함)
    output.position = mul(input.position, W);
    output.position = mul(output.position, g_LightView);
    output.position = mul(output.position, g_LightProj);
    output.clipPos = output.position;
    
    return output;
}

PS_OUT PS_Main(VS_OUT input) : SV_Target
{
    PS_OUT output;
    // 깊이 값(Z/W)을 R 채널에 기록 (0~1 사이 값)
    float depth = input.clipPos.z / input.clipPos.w;
    output.shadowDepth = float4(depth, depth, depth, 1.0f);
    
    return output;
}

technique11 T0
{
    PASS_VP(P0, VS_Main, PS_Main)
    PASS_RS_VP(P1, FrontCounterClockwiseTrue, VS_Main, PS_Main)
};