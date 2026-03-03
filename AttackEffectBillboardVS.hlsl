// AttackEffectBillboardVS.hlsl
// Y軸固定ビルボード頂点シェーダー
// Y軸(上方向)を固定し、XZ平面上でカメラに正対する軸ビルボード

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

    // ワールド中心位置
    float3 worldCenter = float3(world[3][0], world[3][1], world[3][2]);

    // スケール取得
    float scaleX = length(float3(world[0][0], world[0][1], world[0][2]));
    float scaleY = length(float3(world[1][0], world[1][1], world[1][2]));

    float3 up = float3(0.0f, 1.0f, 0.0f);

    // ビュー行列の左3x3転置 = ワールド空間でのカメラ軸
    // camRight: ビュー行列の列0、camForward: ビュー行列の列2
    float3 camRight   = float3(view[0][0], view[1][0], view[2][0]);
    float3 camForward = float3(view[0][2], view[1][2], view[2][2]);

    // Y軸固定: カメラ正面のXZ成分だけ使ってビルボードを向ける
    // (-camForward)のY成分を潰して正規化 → カメラがどこにいても正対する
    float3 look = -camForward;
    look.y = 0.0f;
    look = normalize(look);

    // right は camRight のXZ投影を使う（スケール歪み防止）
    float3 right = camRight;
    right.y = 0.0f;
    right = normalize(right);

    // ビルボード頂点をワールド空間で構築
    float3 worldPos = worldCenter
        + vsin.posL.x * scaleX * right
        + vsin.posL.y * scaleY * up;

    // ビュー → プロジェクション変換
    float4 viewPos = mul(float4(worldPos, 1.0f), view);
    vsout.posH = mul(viewPos, proj);

    vsout.normalW = look;
    vsout.color = vsin.color;
    vsout.uv = vsin.uv;

    return vsout;
}
