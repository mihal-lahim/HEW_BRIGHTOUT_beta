cbuffer PER_FRAME : register(b0)
{
    float4 ambient_light_color;
    float4 directional_light_color;
    float4 directional_light_vector;
}

// cbuffer PER_CAMERA : register(b1)

// cbuffer PER_OBJECT : register(b2)

cbuffer PER_MATERIAL : register(b3)
{
    float4 diffuse_color;
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
    float4 material = major_texture.Sample(major_sampler, psin.uv) * psin.color * diffuse_color;
    float3 ambient = material.rgb * ambient_light_color.rgb;
    
    // -1 ~ 1 -> 0 ~ 2 -> 0 ~ 1
    float brightness = (dot(-directional_light_vector.xyz, normalize(psin.normalW)) + 1) * 0.5f;
        
    float3 diffuse = material.rgb * directional_light_color.rgb * brightness;
        
    float alpha = material.a;
        
    return float4(ambient + diffuse, alpha);
}