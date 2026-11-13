cbuffer PER_MATERIAL : register(b3)
{
    float4 diffuse_color;
    float4 uv_rect;
}

struct PS_INPUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float3 normalW : NORMAL0;
    float2 uv : TEXCOORD0;
};

Texture2D major_texture;
SamplerState major_sampler;

float4 main(PS_INPUT psin) : SV_TARGET
{
    float2 uv = psin.uv * uv_rect.zw + uv_rect.xy;
    // ライティングを無視してテクスチャ色をそのまま出力
    return major_texture.Sample(major_sampler, uv);
}