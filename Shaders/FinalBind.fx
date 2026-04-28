#include "Global.fx"
#include "Light.fx"

// MRT 소스들
Texture2D g_AlbedoTex;
Texture2D g_NormalTex;
Texture2D g_WorldTex;

struct VS_IN
{
    // 엔진의 InputLayout 생성 로직을 통과하기 위해 
    // 기존에 쓰던 구조체의 필드
    float4 position : POSITION;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// 정점 셰이더: vID를 사용해 화면을 덮는 삼각형/사각형 생성
VS_OUT VS_Main(VS_IN input, uint vID : SV_VertexID)
{
    VS_OUT output;

    // vID를 이용한 Full-screen Quad 생성 로직 (Triangle Strip 방식)
    // 0: (-1, 1), 1: (1, 1), 2: (-1, -1), 3: (1, -1)
    float2 uv = float2((vID << 1) & 2, vID & 2);
    output.position = float4(uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    output.uv = uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // MRT 샘플링
    float4 albedo = g_AlbedoTex.Sample(LinearSampler, input.uv);
    float3 normal = g_NormalTex.Sample(LinearSampler, input.uv).xyz;
    float4 worldPositionData = g_WorldTex.Sample(LinearSampler, input.uv);

    // 배경 처리
    if (albedo.a == 0)
        discard;

    // 데이터 복원
    normal = normalize(normal * 2.0f - 1.0f);
    float3 worldPos = worldPositionData.xyz;

    float3 finalColor = albedo.rgb;

    return float4(finalColor, albedo.a);
}

technique11 T0
{
    PASS_VP(P0, VS_Main, PS_Main)
};