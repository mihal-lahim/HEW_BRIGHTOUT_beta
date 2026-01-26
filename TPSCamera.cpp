#include "TPSCamera.h"
#include "player.h"
#include "shader3d.h"
#include <DirectXMath.h>
#include "direct3d.h"
#include "debug_ostream.h"
#include <cmath>
#include <algorithm>
#include "GameObject.h"

using namespace DirectX;


void TPSCamera::Rotate(float inputX, float inputY)
{
	// 水平回転角度を更新（0度～360度の範囲に正規化）
	m_AngleX += inputX * m_Ctx.SensitivityX;
	m_AngleX = fmodf(m_AngleX + 360.0f, 360.0f); // 負の値対応

	// 垂直回転角度を更新（-89度～89度の範囲に制限）
	m_AngleY -= inputY * m_Ctx.SensitivityY;
	m_AngleY = std::clamp(m_AngleY, -89.0f, 89.0f);
}

void TPSCamera::Update()
{
	// 注視対象の位置を取得
	XMFLOAT3 targetPos = m_Target->transform.Position;
	targetPos.y += m_Ctx.Height;
	XMVECTOR targetVec = XMLoadFloat3(&targetPos);

	// ラジアンに変換
	float yaw = XMConvertToRadians(m_AngleX);
	float pitch = XMConvertToRadians(m_AngleY);

	// カメラの相対位置を計算（ターゲットの後方）
	XMVECTOR offset = XMVectorSet(
		Distance * cosf(pitch) * sinf(yaw),
		Distance * sinf(pitch),
		Distance * cosf(pitch) * cosf(yaw),
		0.0f
	);

	// カメラ位置を更新
	XMVECTOR cameraPos = XMVectorAdd(targetVec, offset);
	XMStoreFloat3(&transform.Position, cameraPos);

	// カメラの回転を更新（ターゲットを向く）
	XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(targetVec, cameraPos));
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));
	up = XMVector3Cross(forward, right);

	// 回転行列からQuaternionを生成
	XMMATRIX rotMat;
	rotMat.r[0] = XMVectorSetW(right, 0.0f);
	rotMat.r[1] = XMVectorSetW(up, 0.0f);
	rotMat.r[2] = XMVectorSetW(forward, 0.0f);
	rotMat.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR quat = XMQuaternionRotationMatrix(rotMat);
	XMStoreFloat4(&transform.Rotation.Quat, quat);
}