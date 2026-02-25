cbuffer PerObject : register(b2)
{
    float4x4 world;
};

cbuffer PerCamera : register(b1)
{
    float4x4 projection;
};

cbuffer UIParams : register(b4)
{
    float ScreenWidth;
    float ScreenHeight;
    float UseScreenSpace; 
    float _pad;
};

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUT main(VS_IN input)
{
    VS_OUT o;

    if (UseScreenSpace > 0.5f)
    {
        float2 scale = float2(world._11, world._22);
        float2 trans = float2(world._41, world._42);

        float2 px = input.pos.xy * scale + trans;

        float ndc_x = px.x / ScreenWidth * 2.0f - 1.0f;
        float ndc_y = 1.0f - (px.y / ScreenHeight * 2.0f);

        o.pos = float4(ndc_x, ndc_y, 0.0f, 1.0f);
        o.uv = input.uv;
        return o;
    }
    else
    {
        float4 worldPos = mul(float4(input.pos, 1), world);
        o.pos = mul(worldPos, projection);
        o.uv = input.uv;
        return o;
    }
}