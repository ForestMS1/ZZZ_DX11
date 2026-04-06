#include "Global.fx"
#include "Light.fx"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUT VS(VertexTextureNormalTangent input)
{
    VS_OUT output;
    
    // local -> World -> View -> Proj    
    float4 viewPos = mul(float4(input.position.xyz, 0), V); //xyzw중 w를 0으로 한뒤 V를 곱하는 거
    float4 clipSpacePos = mul(viewPos, P);
    output.position = clipSpacePos.xyzw;
    output.position.z = output.position.w * 0.999999f;
    // xy[-1~1] z[0~1]    
    
    // xyzw
    
    // Rasterizer -> w에다가 z값을 보존해서 
    // x/w y/w z/w 이런 식으로 나눠주는 역할을 한다.
    // x/w y/w z/w 1 이렇게 된다. 
    output.uv = input.uv;
    
    return output;
}

float4 PS(VS_OUT input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    return color;
}

technique11 T0
{
    pass P0
    {
        SetRasterizerState(FrontCounterClockwiseTrue);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
};