// Culling.hlsl

// 메쉬 정보 (CPU에서 StructuredBuffer로 넘겨줌)
struct MeshData
{
    matrix world;
    float3 center;
    float3 extents;
    uint meshIndex; // 어떤 메쉬를 그릴지 식별자
};

// 렌더링에 필요한 인스턴스 정보
struct InstanceData
{
    matrix world;
};

// 입력 및 출력 버퍼
StructuredBuffer<MeshData> InputBuffer : register(t0);
Texture2D<float> HiZMap : register(t1);
SamplerState PointSampler : register(s0);

AppendStructuredBuffer<InstanceData> ResultBuffer : register(u0);

cbuffer CB_Config : register(b0)
{
    matrix ViewProjection;
    float2 ScreenSize;
    uint TotalMeshCount;
}

// Bounding Box를 화면 공간으로 투영하는 함수
bool ProjectAABB(float3 center, float3 extents, out float4 rect, out float minZ)
{
    float3 corners[8] =
    {
        center + extents * float3(-1, -1, -1), center + extents * float3(1, -1, -1),
        center + extents * float3(-1, 1, -1), center + extents * float3(1, 1, -1),
        center + extents * float3(-1, -1, 1), center + extents * float3(1, -1, 1),
        center + extents * float3(-1, 1, 1), center + extents * float3(1, 1, 1)
    };

    float2 minXY = float2(1, 1);
    float2 maxXY = float2(-1, -1);
    minZ = 1.0f;
    bool anyInFront = false;

    for (int i = 0; i < 8; i++)
    {
        float4 clipPos = mul(float4(corners[i], 1.0f), ViewProjection);
        if (clipPos.w > 0)
        {
            anyInFront = true;
            float3 ndc = clipPos.xyz / clipPos.w;
            minXY = min(minXY, ndc.xy);
            maxXY = max(maxXY, ndc.xy);
            minZ = min(minZ, ndc.z);
        }
    }

    if (!anyInFront)
        return false;

    // NDC -> UV
    rect = float4(minXY * 0.5f + 0.5f, maxXY * 0.5f + 0.5f);
    rect.y = 1.0f - rect.y;
    rect.w = 1.0f - rect.w; // Flip Y
    return true;
}

[numthreads(64, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= TotalMeshCount)
        return;

    MeshData mesh = InputBuffer[id.x];
    float4 rect;
    float minZ;

    if (ProjectAABB(mesh.center, mesh.extents, rect, minZ))
    {
        // 프러스텀 컬링 통과 후 오클루전 체크
        float2 size = (rect.zw - rect.xy) * ScreenSize;
        float lod = ceil(log2(max(size.x, size.y)));

        // Hi-Z 맵에서 해당 영역의 최대 깊이 샘플링 (4점 샘플링이 더 정확함)
        float h0 = HiZMap.SampleLevel(PointSampler, rect.xy, lod).r;
        float h1 = HiZMap.SampleLevel(PointSampler, rect.zy, lod).r;
        float h2 = HiZMap.SampleLevel(PointSampler, rect.xw, lod).r;
        float h3 = HiZMap.SampleLevel(PointSampler, rect.zw, lod).r;
        float maxHizDepth = max(max(h0, h1), max(h2, h3));

        // 내 메쉬의 가장 가까운 곳이 벽의 가장 먼 곳보다 앞이면 그린다!
        if (minZ <= maxHizDepth)
        {
            InstanceData outData;
            outData.world = mesh.world;
            ResultBuffer.Add(outData);
        }
    }
}