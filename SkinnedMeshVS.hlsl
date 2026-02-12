// cbuffer PER_FRAME : register(b0)

cbuffer PER_CAMERA : register(b1)
{
    float4x4 view;
    float4x4 proj;
}

cbuffer PER_OBJECT : register(b2)
{
    float4x4 world;
}

// cbuffer PER_MATERIAL : register(b3)

StructuredBuffer<float4x4> Bones : register(t0);

struct VS_INPUT
{
    float3 posL : POSITION0;
    float4 color : COLOR0;
    float3 normalL : NORMAL0;
    float2 uv : TEXCOORD0;
    
    uint4 bone : BONEINDICES;
    float4 weight : BONEWEIGHTS;
};

struct VS_OUTPUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float3 normalW : NORMAL0;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT vsin)
{
    VS_OUTPUT vsout;
    
    float4 localPos = float4(vsin.posL, 1.0f);
    float3 localNormal = vsin.normalL;
    
    float4 skinnedPos = float4(0, 0, 0, 0);
    float3 skinnedNormal = float3(0, 0, 0);

    // ウェイト正規化
    float totalW = vsin.weight.x + vsin.weight.y + vsin.weight.z + vsin.weight.w;
    float4 weights = (totalW > 0.001f) ? (vsin.weight / totalW) : float4(1, 0, 0, 0);

    [unroll]
    for (int i = 0; i < 4; i++)
    {
        float w = weights[i];
        if (w > 0.0f)
        {
            float4x4 bone = Bones[vsin.bone[i]];
            skinnedPos.xyz += w * mul(localPos, bone).xyz;
            skinnedNormal += w * mul(localNormal, (float3x3) bone);
        }
    }
    
    skinnedPos.w = 1.0f;

    // viewプロジェクション変換
    vsout.posH = mul(mul(mul(skinnedPos, world), view), proj);
    
    vsout.normalW = normalize(skinnedNormal);
    vsout.color = vsin.color;
    vsout.uv = vsin.uv;
    
    return vsout;
}