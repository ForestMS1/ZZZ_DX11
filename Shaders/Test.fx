#include "Global.fx"
#include "Light.fx"

float _StariNum = 50;

MeshOutput VS(VertexTextureNormalTangentBlend input)
{
    MeshOutput output;
    output.position = mul(input.position, W);
    output.worldPosition = input.position.xyz;
    output.position = mul(output.position, VP);
    output.clipPos = output.position;
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) W);

    return output;
}

PixelOutput PS(MeshOutput input) : SV_TARGET
{
    //return ComputeLight(input.normal, input.uv, input.worldPosition);
    //return DiffuseMap.Sample(LinearSampler, input.uv);
    
    PixelOutput output;

    // 타겟 0: 디퓨즈 색상
    output.color = DiffuseMap.Sample(LinearSampler, input.uv);
    //output.color = floor(ComputeToonLight(input.normal, input.uv, input.worldPosition) * _StariNum) * (1 / _StariNum); //DiffuseMap.Sample(LinearSampler, input.uv);

    // 타겟 1: 노말 (시각화를 위해 0.5 곱하고 0.5 더함)
    output.normal = float4(normalize(input.normal) * 0.5f + 0.5f, 1.0f);

    // 타겟 2:깊이 값(Z/W)을 R 채널에 기록 (0~1 사이 값)
    //float depth = input.clipPos.z / input.clipPos.w;
    //output.depth = float4(depth, depth, depth, 1.0f);
    output.worldPos = float4(input.worldPosition, 1.0f);

    return output;
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
    PASS_RS_VP(P1, FillModeWireFrame, VS, PS)
};