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
    
    matrix m = GetAnimationMatrix(input);

    output.position = mul(input.position, m);
    //output.position = mul(output.position, W);
    output.position = mul(output.position, W);
    //output.worldPosition = output.position.xyz;
    output.position = mul(output.position, g_LightView);
    output.position = mul(output.position, g_LightProj);
    output.position = float4(output.position.xyz, 1.f);
    output.clipPos = output.position;

    
    return output;
}

VS_OUT VS_Mainee(VertexTextureNormalTangentBlend input)
{
    VS_OUT output;
    
    // 1. 애니메이션 적용 (이게 문제인지 확인)
    matrix m = GetAnimationMatrix(input);
    float4 pos = mul(input.position, m);
    pos = mul(pos, W);

    // 2. 뷰/투영 행렬을 무시하고 NDC 공간에 직접 투사
    // 캐릭터가 모델링된 크기대로 화면 중앙에 나타나야 함
    output.position = float4(pos.xyz, 1.0f);
    output.clipPos = output.position;

    return output;
}

PS_OUT PS_Main(VS_OUT input) : SV_Target
{
    PS_OUT output;
    // 깊이 값(Z/W)을 R 채널에 기록 (0~1 사이 값)
    float depth = input.clipPos.z / input.clipPos.w;
    output.shadowDepth = float4(depth, depth, depth, 1.0f);
    //output.shadowDepth = float4(0, 0, 0, 1.0f);
    return output;
}

technique11 T0
{
    PASS_VP(P0, VS_Main, PS_Main)
    PASS_RS_VP(P1, FrontCounterClockwiseTrue, VS_Main, PS_Main)
};