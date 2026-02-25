#include "Camera.h"
#include "GraphicsDevice.h"
#include "GameObject.h"

using namespace DirectX;


//DirectX::XMMATRIX Camera::GetViewMatrix() const
//{
//	Transform& tf = gameObject().transform();
//
//	// 回転行列を作成
//	XMMATRIX matRot = tf.rotation().ToXMMATRIX();
//
//	// 前方向ベクトルを計算
//	XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), matRot);
//
//	// 上方向ベクトルを計算
//	XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), matRot);
//
//	// カメラ位置を設定
//	XMVECTOR eye = tf.position().ToXMVECTOR();
//
//	// 注視点を計算
//	XMVECTOR target = XMVectorAdd(eye, forward);
//
//	// ビュー行列を作成
//	XMMATRIX view = XMMatrixLookAtLH(eye, target, up);
//
//	return view;
//}
DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	Transform& tf = gameObject().transform();

	// 回転行列を作成
	XMMATRIX matRot = tf.rotation().ToXMMATRIX();

	// 前方向ベクトルを計算
	XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), matRot);

	// 上方向ベクトルを計算
	XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), matRot);

	// プレイヤー（注視点）位置
	XMVECTOR target = tf.position().ToXMVECTOR();

	// カメラとプレイヤー間の距離（正の値で後方にオフセット）
	// 値を大きくするとプレイヤーからより遠くなる（必要に応じて調整 / Camera クラスのメンバに移すことを推奨）
	const float FollowDistance = 10.0f;

	// カメラ位置をプレイヤー位置の後方に設定（プレイヤーの前方向に対して後ろへオフセット）
	XMVECTOR eye = XMVectorSubtract(target, XMVectorScale(forward, FollowDistance));

	// ビュー行列を作成
	XMMATRIX view = XMMatrixLookAtLH(eye, target, up);

	return view;
}

DirectX::XMMATRIX Camera::GetProjectionMatrix(float defaultWidth, float defaultHeight) const
{
	// アスペクト比を計算
	float aspect = ViewportWidth == -1.0f || ViewportHeight == -1.0f
		? defaultWidth / defaultHeight
		: ViewportWidth / ViewportHeight;

	// プロジェクション行列を作成
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(Fov), aspect, Near, Far);

	return proj;
}

DirectX::XMMATRIX Camera::GetUIProjectionMatrix(float width, float height) const
{
	// 画面左上(0,0)、右下(width,height)の正射影
	return XMMatrixOrthographicOffCenterLH(
		0.0f,        // left
		width,       // right
		height,      // bottom
		0.0f,        // top（Y反転）
		0.0f,
		1.0f
	);
}
