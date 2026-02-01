#include "TPSCamera.h"
#include "player.h"
#include "shader3d.h"
#include <DirectXMath.h>
#include "direct3d.h"
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
	m_AngleY = std::clamp(m_AngleY, -85.0f, 85.0f);
}

void TPSCamera::Update()
{
	// 注視対象の位置を取得
	Vector3 targetPos = m_Target->transform().position();
	targetPos.y += m_Ctx.Height;

	// ラジアンに変換
	float yaw = XMConvertToRadians(m_AngleX);
	float pitch = XMConvertToRadians(m_AngleY);

	// カメラの相対位置を計算（ターゲットの後方）
	Vector3 offset = {
		Distance * cosf(pitch) * sinf(yaw),
		Distance * sinf(pitch),
		Distance * cosf(pitch) * cosf(yaw)
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