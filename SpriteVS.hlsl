cbuffer PER_CAMERA : register(b0)
{
    float4x4 proj;
};

cbuffer PER_OBJECT : register(b1)
{
    float4x4 world;
};

struct VS_INPUT
{
    float4 posL     : POSITION0;
    float4 color    : COLOR0;
    float2 uv       : TEXCOORD0;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

VS_OUT main(VS_INPUT vsin)
{
    VS_OUT vsout;
    
    float4x4 mtx = mul(world,proj);
    vsout.posH = mul(vsin.posL, mtx);
    
    vsout.color = vsin.color;
    vsout.uv = vsin.uv;
    
    return vsout;
}
