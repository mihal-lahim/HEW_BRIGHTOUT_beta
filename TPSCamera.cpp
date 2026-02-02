#include "TPSCamera.h"
#include "player.h"
#include "shader3d.h"
#include <DirectXMath.h>
#include "GraphicsDevice.h"
#include <cmath>
#include <algorithm>
#include "GameObject.h"

using namespace DirectX;


void TPSCamera::Rotate(float inputX, float inputY)
{
	// 水平回転角度を更新（0度～360度の範囲に正規化）
	m_angleX += inputX * m_ctx.SensitivityX;
	m_angleX = fmodf(m_angleX + 360.0f, 360.0f); // 負の値対応

	// 垂直回転角度を更新（-89度～89度の範囲に制限）
	m_angleY -= inputY * m_ctx.SensitivityY;
	m_angleY = std::clamp(m_angleY, -89.0f, 89.0f);
}

void TPSCamera::Update()
{
	// 注視対象の位置を取得
	Vector3 targetPos = m_target->transform().position();
	targetPos.y += m_ctx.Height;

	// ラジアンに変換
	float yaw = XMConvertToRadians(m_angleX);
	float pitch = XMConvertToRadians(m_angleY);

	// カメラの相対位置を計算（ターゲットの後方）
	Vector3 offset = {
		m_ctx.Distance * cosf(pitch) * sinf(yaw),
		m_ctx.Distance * sinf(pitch),
		m_ctx.Distance * cosf(pitch) * cosf(yaw)
	};

	// カメラ位置を更新
	Vector3 cameraPos = targetPos + offset;
	gameObject().transform().position() = cameraPos;

	// カメラの回転を更新（ターゲットを向く）
	Vector3 forward = (targetPos - cameraPos).Normalize();
	Vector3 up = { 0.0f, 1.0f, 0.0f };
	Vector3 right = up.CrossVector(forward).Normalize();
	up = forward.CrossVector(right);

	// 回転行列からQuaternionを生成
	XMMATRIX rotMat;
	rotMat.r[0] = XMVectorSet(right.x, right.y, right.z, 0.0f);
	rotMat.r[1] = XMVectorSet(up.x, up.y, up.z, 0.0f);
	rotMat.r[2] = XMVectorSet(forward.x, forward.y, forward.z, 0.0f);
	rotMat.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	gameObject().transform().rotation().FromXMMATRIX(rotMat);
}