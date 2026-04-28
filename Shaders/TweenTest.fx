#include "Global.fx"
#include "Light.fx"

#define MAX_MODEL_TRANSFORMS 350
#define MAX_MODEL_KEYFRAMES 700

struct KeyframeDesc
{
    int animIndex;
    uint currFrame;
    uint nextFrame;
    float ratio;
    float sumTime;
    float speed;
    float2 padding;
};

struct TweenFrameDesc
{
    float tweenDuration;
    float tweenRatio;
    float tweenSumTime;
    float padding;
    KeyframeDesc curr;
    KeyframeDesc next;
};

cbuffer TweenBuffer
{
    TweenFrameDesc TweenFrames;
};

cbuffer BoneBuffer
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
};

uint BoneIndex;
Texture2DArray TransformMap;

matrix GetAnimationMatrix(VertexTextureNormalTangentBlend input)
{
    float indices[4] = { input.blendIndices.x, input.blendIndices.y, input.blendIndices.z, input.blendIndices.w };
    float weights[4] = { input.blendWeights.x, input.blendWeights.y, input.blendWeights.z, input.blendWeights.w };

    int animIndex[2];
    int currFrame[2];
    int nextFrame[2];
    float ratio[2];

    animIndex[0] = TweenFrames.curr.animIndex;
    currFrame[0] = TweenFrames.curr.currFrame;
    nextFrame[0] = TweenFrames.curr.nextFrame;
    ratio[0] = TweenFrames.curr.ratio;

    animIndex[1] = TweenFrames.next.animIndex;
    currFrame[1] = TweenFrames.next.currFrame;
    nextFrame[1] = TweenFrames.next.nextFrame;
    ratio[1] = TweenFrames.next.ratio;

    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    matrix curr = 0;
    matrix next = 0;
    matrix transform = 0;

    for (int i = 0; i < 4; i++)
    {
        c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
        c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
        c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));
        c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0));
        curr = matrix(c0, c1, c2, c3);

        n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
        n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
        n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
        n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
        next = matrix(n0, n1, n2, n3);

        matrix result = lerp(curr, next, ratio[0]);

		// 다음 애니메이션
        if (animIndex[1] >= 0)
        {
            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
            curr = matrix(c0, c1, c2, c3);

            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
            next = matrix(n0, n1, n2, n3);

            matrix nextResult = lerp(curr, next, ratio[1]);
            result = lerp(result, nextResult, TweenFrames.tweenRatio);
        }

        transform += mul(weights[i], result);
    }

    return transform;
}

MeshOutput VS(VertexTextureNormalTangentBlend input)
{
    MeshOutput output;

	// TODO
    matrix m = GetAnimationMatrix(input);

    output.position = mul(input.position, m);
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    output.position = mul(output.position, VP);
    output.clipPos = output.position;
    output.uv = input.uv;
    
    // 애니메이션 행렬 m을 먼저 곱한 뒤 월드 행렬을 곱해야 함.
    output.normal = mul(input.normal, (float3x3) m);
    output.normal = mul(output.normal, (float3x3) W);
    
    output.tangent = mul(input.tangent, (float3x3) m);
    output.tangent = mul(output.tangent, (float3x3) W);

    return output;
}

float _StariNum = 50;

PixelOutput PS(MeshOutput input) : SV_TARGET
{
	//ComputeNormalMapping(input.normal, input.tangent, input.uv);
	//float4 color = ComputeLight(input.normal, input.uv, input.worldPosition);
   // float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
   // 
   //
   // return color;
    
    // 노멀 확인 디버그
    //float3 debugNormal = normalize(input.normal);
    //return float4(debugNormal * 0.5f + 0.5f, 1.0f);
    
    PixelOutput output;

    // 타겟 0: 디퓨즈 색상
    output.color = DiffuseMap.Sample(LinearSampler, input.uv);
    //output.color = floor(ComputeToonLight(input.normal, input.uv, input.worldPosition) * _StariNum) * (1 / _StariNum);

    // 타겟 1: 노말 (시각화를 위해 0.5 곱하고 0.5 더함)
    output.normal = float4(normalize(input.normal) * 0.5f + 0.5f, 1.0f);

    // 타겟 2: 월드
    output.world = float4(input.worldPosition.xyz * 0.01f, 1.0f);
    
    return output;
}

float4 PS_RED(MeshOutput input) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
    PASS_RS_VP(P1, FillModeWireFrame, VS, PS)
	PASS_RS_VP(P2, FillModeWireFrame, VS, PS_RED)
};
