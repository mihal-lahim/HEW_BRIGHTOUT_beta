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

    // ウェイトを事前に正規化
    float totalW = dot(vsin.weight, float4(1, 1, 1, 1));
    float4 weights = vsin.weight / (totalW > 0 ? totalW : 1.0f);

    // ループを使わずに計算（展開）
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        if (weights[i] > 0.0f)
        {
            // 座標変換
            skinnedPos += weights[i] * mul(localPos, Bones[vsin.bone[i]]);
            // 法線変換（ここではまだ normalize しない）
            skinnedNormal += weights[i] * mul(localNormal, (float3x3) Bones[vsin.bone[i]]);
        }
    }

    // 正規化と w の修正
    skinnedPos.w = 1.0f;
    skinnedNormal = normalize(skinnedNormal);

    // WVP変換
    float4x4 mtxWVP = mul(mul(world, view), proj);
    vsout.posH = mul(skinnedPos, mtxWVP);
    
    // 法線の最終変換（ワールド行列を適用）
    vsout.normalW = normalize(mul(skinnedNormal, (float3x3) world));
    
    vsout.color = vsin.color;
    vsout.uv = vsin.uv;
    
    return vsout;
}