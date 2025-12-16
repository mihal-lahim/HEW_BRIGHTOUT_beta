#include "checkmark.h"

#include "sprite.h"
#include <DirectXMath.h>
#include <cmath>

int CheckMark::s_texture = -1;

CheckMark::CheckMark() = default;
CheckMark::~CheckMark() = default;

void CheckMark::Draw(const XMFLOAT3& housePos, float houseScale/*, const XMFLOAT3& playerPos*/) const
{
    if (!s_texture) return;

    /*
    // プレイヤー方向に向くビルボード計算
    XMVECTOR dir = XMVectorSubtract(XMLoadFloat3(&playerPos), XMLoadFloat3(&housePos));
    dir = XMVectorSetY(dir, 0); // 水平のみ
    XMVECTOR forward = XMVector3Normalize(dir);
    float angleY = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));
    */

    // 家の上にチェックマークを浮かせる
    XMFLOAT3 markPos = {
        housePos.x,
        housePos.y + houseScale + m_heightOffset,
        housePos.z
    };
    Sprite_Draw(s_texture, markPos.x, markPos.z, m_size, { 1.0f,1.0f,1.0f,1.0f });
}