cbuffer PER_MATERIAL : register(b0)
{
    float4 material_diffuse_color;
};

struct PS_INPUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D major_texture;
SamplerState major_sampler;

float4 main(PS_INPUT psin): SV_TARGET
{
    float4 texcolor = major_texture.Sample(major_sampler, psin.uv);
    
    return psin.color * texcolor * material_diffuse_color;
}