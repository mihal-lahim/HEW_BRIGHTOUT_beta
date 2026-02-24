cbuffer PerObject : register(b2)
{
    float4x4 world;
};

cbuffer PerCamera : register(b1)
{
    float4x4 view;
    float4x4 projection;
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

    float4 worldPos = mul(float4(input.pos, 1), world);
    float4 viewPos = mul(worldPos, view);
    o.pos = mul(viewPos, projection);

    o.uv = input.uv;
    return o;
}