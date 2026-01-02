#include "camera.h"
#include "direct3d.h"
#include "shader3d.h"

using namespace DirectX;

void Camera::SetMatrix() const
{
	// 回転をオイラー角からクォータニオンに変換
	XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_Rotation.x),
		XMConvertToRadians(m_Rotation.y),
		XMConvertToRadians(m_Rotation.z)
	);

	// クォータニオンから回転行列を作成
	XMMATRIX matRot = XMMatrixRotationQuaternion(rotation);

	// 前方向ベクトルを計算
	XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), matRot);

	// 上方向ベクトルを計算
	XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), matRot);

	// カメラ位置を設定
	XMVECTOR eye = XMLoadFloat3(&m_Position);

	// 注視点を計算
	XMVECTOR target = XMVectorAdd(eye, forward);

	// ビュー行列を作成
	XMMATRIX view = XMMatrixLookAtLH(eye, target, up);

	// シェーダにビュー行列を設定
	Shader3d_SetViewMatrix(view);



	// アスペクト比を計算
	float aspect = m_AspectRatio == -1.0f ? (float)Direct3D_GetBackBufferWidth() / (float)Direct3D_GetBackBufferHeight() : m_AspectRatio;

	// プロジェクション行列を作成
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_Fov), aspect, m_Near, m_Far);

	// シェーダにプロジェクション行列を設定
	Shader3d_SetProjectionMatrix(proj);
}
