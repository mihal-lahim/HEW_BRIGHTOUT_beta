// BillboardPS.hlsl
// ビルボード用ピクセルシェーダー（ライティングなし・テクスチャ×色）

cbuffer PER_MATERIAL : register(b3)
{
    float4 diffuse_color;
    float4 uv_rect;
    float uv_rotation;
    float3 _padding0;
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
    float s = sin(uv_rotation);
    float c = cos(uv_rotation);
    float2 centered = psin.uv - float2(0.5f, 0.5f);
    float2 rotated = float2(
        centered.x * c - centered.y * s,
        centered.x * s + centered.y * c
    ) + float2(0.5f, 0.5f);

    float2 uv = rotated * uv_rect.zw + uv_rect.xy;
    float4 color = major_texture.Sample(major_sampler, uv) * psin.color * diffuse_color;
    clip(color.a - 0.01f);
    return color;
}
