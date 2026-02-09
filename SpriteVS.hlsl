// cbuffer PER_FRAME : register(b0)

cbuffer PER_CAMERA : register(b1)
{
    float4x4 proj;
};

cbuffer PER_OBJECT : register(b2)
{
    float4x4 world;
};

// cbuffer PER_MATERIAL : register(b3)

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
