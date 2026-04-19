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

