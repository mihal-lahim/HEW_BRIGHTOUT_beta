#include "camera.h"
#include "direct3d.h"
#include "shader3d.h"

using namespace DirectX;

void Camera::SetMatrix() const
{
	// 所有者のゲームオブジェクトを取得
	GameObject* owner = GetOwner();

	Transform& transform = owner->Transform;

	// 回転行列を作成
	XMMATRIX matRot = XMMatrixRotationQuaternion(XMLoadFloat4(&transform.Rotation.Quat));

	// 前方向ベクトルを計算
	XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), matRot);

	// 上方向ベクトルを計算
	XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), matRot);

	// カメラ位置を設定
	XMVECTOR eye = XMLoadFloat3(&transform.Position);

	// 注視点を計算
	XMVECTOR target = XMVectorAdd(eye, forward);

	// ビュー行列を作成
	XMMATRIX view = XMMatrixLookAtLH(eye, target, up);

	// シェーダにビュー行列を設定
	Shader3d_SetViewMatrix(view);



	// アスペクト比を計算
	float aspect = ViewportWidth == -1.0f || ViewportHeight == -1.0f
		? (float)Direct3D_GetBackBufferWidth() / (float)Direct3D_GetBackBufferHeight() : ViewportWidth / ViewportHeight;

	// プロジェクション行列を作成
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(Fov), aspect, Near, Far);

	// シェーダにプロジェクション行列を設定
	Shader3d_SetProjectionMatrix(proj);
}
