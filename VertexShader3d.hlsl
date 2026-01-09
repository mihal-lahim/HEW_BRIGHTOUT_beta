cbuffer VS_CONSTANT_BUFFER0 : register(b0)
{
    float4x4 world;
};

cbuffer VS_CONSTANT_BUFFER1 : register(b1)
{
    float4x4 view;
};

cbuffer VS_CONSTANT_BUFFER2 : register(b2)
{
    float4x4 proj;
};

// 頂点入力構造体
struct VS_INPUT
{
    float3 posL     : POSITION0;
    float3 normalL  : NORMAL0;
    float4 color    : COLOR0;
    float2 uv       : TEXCOORD0;
    uint4 boneIndex : BLENDINDICES0;
    float4 boneWeight : BLENDWEIGHT0;
};
// 頂点シェーダー出力構造体
struct VS_OUT
{
    float4 posH     : SV_POSITION;
    float3 normalW  : NORMAL0;
    float4 color    : COLOR0;
    float2 uv       : TEXCOORD0;
};

VS_OUT main(VS_INPUT vsin)
{
    VS_OUT vsout;
    
    // ボーンインデックスとウェイトは利用可能だが、基本的なレンダリングでは未使用
    // 将来のスケルタルアニメーション実装で使用予定
    
    // 行列演算による変換
    float4x4 mtxWV = mul(world,view);
    float4x4 mtxWVP = mul(mtxWV, proj);
    vsout.posH = mul(float4(vsin.posL, 1.0f), mtxWVP); // mul は行列の積
    
    // ワールド座標での法線変換
    // 法線はカメラライト、ローカル法線でのライティング(略した?)
    // ア 行列 オフセット(転置)行列でワールド法線変換(略した?)
    
    vsout.normalW = mul(float4(vsin.normalL,0.0f), world).xyz;
    
    vsout.color = vsin.color;
    vsout.uv = vsin.uv;
    
    return vsout;
}
