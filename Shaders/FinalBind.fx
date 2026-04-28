#include "Global.fx"
#include "Light.fx"

// MRT 소스들
Texture2D g_AlbedoTex;
Texture2D g_NormalTex;
Texture2D g_DepthTex;

Texture2D g_ShadowTex;

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
    // 1. MRT 샘플링
    float4 albedo = g_AlbedoTex.Sample(LinearSampler, input.uv);
    float3 normal = g_NormalTex.Sample(LinearSampler, input.uv).xyz;
    float4 worldPosData = g_DepthTex.Sample(LinearSampler, input.uv);

    if (albedo.a == 0)
        discard;

    // 2. 데이터 복원
    normal = normalize(normal * 2.0f - 1.0f);
    float3 worldPos = worldPosData.xyz;
    
    // 3. 그림자 판정 (Shadow Mapping)
    float shadowFactor = 1.0f;
    
    // [수정] 월드 좌표 -> 광원 View -> 광원 Proj (순차적으로 곱해야 합니다)
    float4 shadowPos = mul(float4(worldPos, 1.0f), g_LightView);
    shadowPos = mul(shadowPos, g_LightProj);
    
    // NDC 좌표로 변환 (W로 나누기)
    // Directional Light(직교투영)의 경우 W가 1일 수도 있지만, 
    // 범용성을 위해 나누는 과정을 거칩니다.
    float3 ndcPos = shadowPos.xyz / shadowPos.w;
    
    // NDC (-1~1) -> UV (0~1) 변환
    float2 shadowUV = ndcPos.xy * 0.5f + 0.5f;

    // 광원 범위(NDC 상의 Z축 포함) 안에 있는 픽셀만 계산
    if (shadowUV.x >= 0 && shadowUV.x <= 1 && shadowUV.y >= 0 && shadowUV.y <= 1)
    {
        // [주의] 현재 픽셀의 깊이값은 ndcPos.z를 사용합니다.
        float currentDepth = ndcPos.z;
        
        float shadowDepth = g_ShadowTex.SampleCmpLevelZero(ShadowSampler, shadowUV, currentDepth).r;

        // Shadow Acne 방지를 위한 Bias
        if (currentDepth > shadowDepth + 0.01f)
        {
            shadowFactor = 0.5f;
        }
    }
    

    // 4. 최종 색상 결정
    // 나중에 ComputeToonLight 함수가 완성되면 여기에 shadowFactor를 곱하세요.
    float3 finalColor = ComputeToonDefferedLight(albedo, normal, input.uv, worldPos) * shadowFactor;
    
   return float4(finalColor, albedo.a);
}

technique11 T0
{
    PASS_VP(P0, VS_Main, PS_Main)
};