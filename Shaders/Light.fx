#ifndef _LIGHT_FX_
#define _LIGHT_FX_

#include "Global.fx"

////////////
// Struct //
////////////

struct LightDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
    float3 direction;
    float padding;
};

struct MaterialDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
};

/////////////////
// ConstBuffer //
/////////////////

cbuffer LightBuffer
{
    LightDesc GlobalLight;
};

cbuffer MaterialBuffer
{
    MaterialDesc Material;
};

/////////
// SRV //
/////////

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;

//////////////
// Function //
//////////////

float4 ComputeLight(float3 normal, float2 uv, float3 worldPosition)
{
    float4 ambientColor = 0;
    float4 diffuseColor = 0;
    float4 specularColor = 0;

    // 준비: 모든 벡터는 다시 한 번 정규화 (보간 과정에서 길이가 변함)
    float3 N = normalize(normal);
    float3 L = normalize(-GlobalLight.direction); // 광원을 향하는 벡터
    float4 texColor = DiffuseMap.Sample(LinearSampler, uv);

    // Ambient
    // Material.ambient가 (1,1,1,1)인지 꼭 확인
    ambientColor = texColor * GlobalLight.ambient * Material.ambient;

    // Diffuse
    float dotNL = dot(N, L);
    float diffuseFactor = saturate(dotNL); // 음수 방지 (0~1)
    

    diffuseColor = texColor * diffuseFactor * GlobalLight.diffuse * Material.diffuse;

    // Specular 정반사
    float3 viewDir = normalize(CameraPosition() - worldPosition);
    float3 halfWay = normalize(L + viewDir); // 하프 벡터
    float specFactor = pow(saturate(dot(N, halfWay)), 32); // 32는 빛나는 정도
    specularColor = GlobalLight.specular * Material.specular * specFactor;
   

    // 결과 합산
    return ambientColor + diffuseColor + specularColor;
}

float4 ComputeToonLight(float3 normal, float2 uv, float3 worldPosition)
{
    float4 ambientColor = 0;
    float4 diffuseColor = 0;
    float4 specularColor = 0;

    // 준비: 모든 벡터는 다시 한 번 정규화 (보간 과정에서 길이가 변함)
    float3 N = normalize(normal);
    float3 L = normalize(-GlobalLight.direction); // 광원을 향하는 벡터
    float4 texColor = DiffuseMap.Sample(LinearSampler, uv);

    // Ambient
    // Material.ambient가 (1,1,1,1)인지 꼭 확인
    ambientColor = texColor * GlobalLight.ambient * Material.ambient;

    // Diffuse
    float dotNL = dot(N, L);
    float diffuseFactor = saturate(dotNL); // 음수 방지 (0~1)
    float halfLambert = pow(dotNL * 0.5f + 0.5f, 2.0f);
    
    // [Smoothstep 기반 3단계 음영 로직]
    // feather: 경계면의 부드러운 정도 (0.01~0.05 사이 추천)
    float feather = 0.05f;
    
    // 두 개의 경계면 마스크 생성 (0 또는 1에 수렴)
    float mask1 = smoothstep(0.4f - feather, 0.4f + feather, halfLambert); // 0.4 지점 경계
    float mask2 = smoothstep(0.7f - feather, 0.7f + feather, halfLambert); // 0.7 지점 경계

    // 최종 음영 비율 계산: 기본 0.3에서 시작해서 마스크가 찰 때마다 밝기 추가
    // 0.3 (그늘) -> 0.6 (중간) -> 1.0 (밝음)
    float threeStep = 0.3f + (mask1 * 0.3f) + (mask2 * 0.4f);

    diffuseColor = texColor * threeStep * GlobalLight.diffuse * Material.diffuse;

    //diffuseColor = texColor * diffuseFactor * GlobalLight.diffuse * Material.diffuse;
    //diffuseColor = texColor * halfLambert * GlobalLight.diffuse * Material.diffuse;

    // Specular 정반사
    float3 viewDir = normalize(CameraPosition() - worldPosition);
    float3 halfWay = normalize(L + viewDir); // 하프 벡터
    float specFactor = pow(saturate(dot(N, halfWay)), 128); // 128는 빛나는 정도
    
    // [카툰 스펙큘러 처리]
    // 하이라이트 경계가 0.5보다 크면 1, 작으면 0으로 딱 끊어버림
    float specFeather = 0.01f; // 스펙큘러 경계의 부드러움
    float toonSpec = smoothstep(0.5f - specFeather, 0.5f + specFeather, specFactor);

    // 최종 스펙큘러 컬러
    specularColor = GlobalLight.specular * Material.specular * toonSpec;
    //specularColor = GlobalLight.specular * Material.specular * specFactor;

    // 결과 합산
    return ambientColor + diffuseColor + specularColor;
}


void ComputeNormalMapping(inout float3 normal, float3 tangent, float2 uv)
{
	// [0,255] 범위에서 [0,1]로 변환
    float4 map = NormalMap.Sample(LinearSampler, uv);
    if (any(map.rgb) == false)
        return;

    float3 N = normalize(normal); // z
    float3 T = normalize(tangent); // x
    float3 B = normalize(cross(T, N)); // y
    float3x3 TBN = float3x3(T, B, N); // TS -> WS

	// [0,1] 범위에서 [-1,1] 범위로 변환
    float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
    float3 worldNormal = tangentSpaceNormal.x * T;
    worldNormal += tangentSpaceNormal.y * B;
    worldNormal += tangentSpaceNormal.z * N;
    
    normal = normalize(worldNormal);
}

#endif

