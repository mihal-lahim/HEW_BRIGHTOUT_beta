#include "tps_camera.h"
#include "player.h"
#include "shader3d.h"
#include <DirectXMath.h>
#include "direct3d.h"
#include "debug_ostream.h"
#include <cmath>
#include <algorithm>

using namespace DirectX;


void TPSCamera::Rotate(float inputX, float inputY)
{
	// 注視対象の位置を取得
	XMFLOAT3 targetPos = m_Target->GetPosition();

	// 高さ調整
	targetPos.y += m_Ctx.Height;

	// 注視対象位置ベクトル
	XMVECTOR targetVec = XMLoadFloat3(&targetPos);

	// 水平回転角度を更新（0度～360度の範囲に制限）
	m_AngleX += inputX * m_Ctx.SensitivityX;
	m_AngleX = fmodf(m_AngleX, 360.0f);
	float yaw = XMConvertToRadians(m_AngleX);

	// 垂直回転角度を更新（-89度～89度の範囲に制限）
	m_AngleY += inputY * m_Ctx.SensitivityY;
	m_AngleY = std::clamp(m_AngleY, -89.0f, 89.0f);
	float pitch = XMConvertToRadians(m_AngleY);

	// カメラの相対位置を計算
	XMVECTOR offset = XMVectorSet(
		Distance * cosf(pitch) * sinf(yaw),
		Distance * sinf(pitch),
		Distance * cosf(pitch) * cosf(yaw),
		0.0f
	);

	// 目標位置を計算
	m_Dest = XMVectorAdd(targetVec, offset);
}

void TPSCamera::Update(double)
{
	// 位置を更新
	XMStoreFloat3(&m_Position, m_Dest);
}