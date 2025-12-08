#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

struct TEXTURE;

using namespace DirectX;

class CheckMark
{
public:
    CheckMark();
    ~CheckMark();

    // 静的テクスチャ設定（最初に1回だけ）
    static void SetTexture(TEXTURE* texture) { s_texture = texture; }

    // チェックマーク描画
    // housePos: 家の位置
    // houseScale: 家のスケール
    // playerPos: プレイヤー位置（向きを計算）
    void Draw(const XMFLOAT3& housePos, float houseScale, const XMFLOAT3& playerPos) const;

private:
    static TEXTURE* s_texture;
    float m_heightOffset = 1.0f; // 家の上に浮かせる高さ
    float m_size = 1.0f;         // チェックマークのサイズ
};