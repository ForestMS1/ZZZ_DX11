

Texture2D<float> ParentDepth : register(t0);
RWTexture2D<float> ChildDepth : register(u0);

[numthreads(16, 16, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    float2 srcSize;
    ParentDepth.GetDimensions(srcSize.x, srcSize.y);
    
    // 현재 픽셀이 담당하는 이전 단계의 2x2 픽셀 위치 계산
    uint2 base = id.xy * 2;
    
    float d1 = ParentDepth.Load(uint3(base + uint2(0, 0), 0));
    float d2 = ParentDepth.Load(uint3(base + uint2(1, 0), 0));
    float d3 = ParentDepth.Load(uint3(base + uint2(0, 1), 0));
    float d4 = ParentDepth.Load(uint3(base + uint2(1, 1), 0));
    
    // Max Depth를 선택 (D3D11 기준 1.0이 먼 쪽, 0.0이 가까운 쪽인 경우)
    float maxDepth = max(max(d1, d2), max(d3, d4));
    
    ChildDepth[id.xy] = maxDepth;
}