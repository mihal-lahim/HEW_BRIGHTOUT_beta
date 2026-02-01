#include "Camera.h"
#include "direct3d.h"
#include "GameObject.h"

using namespace DirectX;


DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	Transform& tf = gameObject().transform();

	// 回転行列を作成
	XMMATRIX matRot = tf.rotation().ToXMMATRIX();

	// 前方向ベクトルを計算
	XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), matRot);

	// 上方向ベクトルを計算
	XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), matRot);

	// カメラ位置を設定
	XMVECTOR eye = tf.position().ToXMVECTOR();

	// 注視点を計算
	XMVECTOR target = XMVectorAdd(eye, forward);

	// ビュー行列を作成
	XMMATRIX view = XMMatrixLookAtLH(eye, target, up);

	return view;
}

DirectX::XMMATRIX Camera::GetProjectionMatrix() const
{
	// アスペクト比を計算
	float aspect = ViewportWidth == -1.0f || ViewportHeight == -1.0f
		? (float)Direct3D_GetBackBufferWidth() / (float)Direct3D_GetBackBufferHeight() : ViewportWidth / ViewportHeight;

	// プロジェクション行列を作成
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(Fov), aspect, Near, Far);

	return proj;
}
