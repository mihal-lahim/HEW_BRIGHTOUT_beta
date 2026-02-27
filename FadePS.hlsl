Texture2D tex : register(t0);
SamplerState samp : register(s0);

cbuffer PER_MATERIAL : register(b3)
{
    float4 diffuse_color;
    float4 uv_rect;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PS_IN input) : SV_Target
{
    float4 texColor = tex.Sample(samp, input.uv);
    // テクスチャ色にマテリアルカラー(diffuse_color)を乗算
    // 白テクスチャ × (0,0,0,alpha) = 黒いオーバーレイ（alphaで透明度制御）
    return texColor * diffuse_color;
}