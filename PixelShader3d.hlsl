cbuffer PS_CONSTANT_BUFFER0 : register(b0)
{
    float4 ambient_color;
};

cbuffer PS_CONSTANT_BUFFER1 : register(b1)
{
    float4 diffuse_color;
    float4 diffuse_world_vector;
};

cbuffer PS_CONSTANT_BUFFER2 : register(b2)
{
    float4 material_diffuse_color;
};

cbuffer PS_CONSTANT_BUFFER3 : register(b3)
{
    float4 override_color;
    int use_override;
    int has_tex;
    int pad[2];
};

// ピクセルシェーダー入力構造体
struct PS_INPUT
{
    float4 posH      : SV_POSITION;
    float3 normalW   : NORMAL0;
    float4 color     : COLOR0;
    float2 uv        : TEXCOORD0;
};

Texture2D major_texture; // テクスチャ
SamplerState major_sampler; // テクスチャサンプラー

float4 main(PS_INPUT psin): SV_TARGET
{
    float3 base_color;

    // 基本色の決定: オーバーライド > テクスチャ > マテリアル
    if (use_override)
    {
        base_color = override_color.rgb;
    }
    else if (has_tex)
    {
        base_color = major_texture.Sample(major_sampler, psin.uv).rgb;
    }
    else
    {
        base_color = material_diffuse_color.rgb;
    }

    float4 material = float4(base_color, 1.0f) * psin.color;
    float3 ambient = material.rgb * ambient_color.rgb;
    
    // 明度の計算（-1～1 を 0～1 に正規化）
    float brightness = (dot(-diffuse_world_vector.xyz, normalize(psin.normalW)) + 1.0f) * 0.5f;
    float3 diffuse = material.rgb * diffuse_color.rgb * brightness;
    
    float alpha = material.a * ambient_color.a;
    
    return float4(ambient + diffuse, alpha);
}
