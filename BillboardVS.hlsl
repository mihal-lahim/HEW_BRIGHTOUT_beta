// BillboardVS.hlsl
// フルビルボード頂点シェーダー
// view行列からカメラのright/upベクトルを取得し、板を常にカメラに向ける

cbuffer PER_CAMERA : register(b1)
{
    float4x4 view;
    float4x4 proj;
}

cbuffer PER_OBJECT : register(b2)
{
    float4x4 world;
}

struct VS_INPUT
{
    float3 posL : POSITION0;
    float4 color : COLOR0;
    float3 normalL : NORMAL0;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float3 normalW : NORMAL0;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT vsin)
{
    VS_OUTPUT vsout;

    // world行列からワールド座標（平行移動成分）を取得
    float3 worldCenter = float3(world[3][0], world[3][1], world[3][2]);

    // world行列からスケールを取得（各軸のベクトル長）
    float scaleX = length(float3(world[0][0], world[0][1], world[0][2]));
    float scaleY = length(float3(world[1][0], world[1][1], world[1][2]));

    // view行列からカメラのright/upベクトルを取得
    // view行列の列0 = カメラ右方向、列1 = カメラ上方向（ワールド空間）
    float3 camRight = float3(view[0][0], view[1][0], view[2][0]);
    float3 camUp    = float3(view[0][1], view[1][1], view[2][1]);

    // ビルボード頂点をワールド空間で構築
    float3 worldPos = worldCenter
        + vsin.posL.x * scaleX * camRight
        + vsin.posL.y * scaleY * camUp;

    // ビュー → プロジェクション変換
    float4 viewPos = mul(float4(worldPos, 1.0f), view);
    vsout.posH = mul(viewPos, proj);

    // 法線はカメラ正面方向（ライティング用）
    float3 camForward = float3(view[0][2], view[1][2], view[2][2]);
    vsout.normalW = -camForward;

    vsout.color = vsin.color;
    vsout.uv = vsin.uv;

    return vsout;
}
