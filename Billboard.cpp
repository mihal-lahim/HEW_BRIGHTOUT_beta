#include "Billboard.h"
#include <DirectXMath.h>
#include "GameObject.h"

using namespace DirectX;

void Billboard::Awake()
{
	m_mainCamera = GetGameObjectByTag("MainCamera");
}

void Billboard::Update()
{
	if (!m_mainCamera) return;
	// 自身のTransform取得
	Transform& myTransform = gameObject().transform();
	// カメラのTransform取得
	Transform& camTransform = m_mainCamera->transform();

	// カメラの位置と自身の位置から向きを計算
	Vector3 direction = myTransform.position() - camTransform.position();

	float dx = direction.x;
	float dz = direction.z;

	// 左右回転
	float rotY = atan2f(dx, dz);

	float dy = direction.y;
	float horizonal = sqrtf(dx * dx + dz * dz);

	// 上下回転
	float rotX = atan2f(dy, horizonal) + XM_PIDIV2;

	XMMATRIX rotYMat = XMMatrixRotationX(-rotX) * XMMatrixRotationY(rotY);

	myTransform.rotation().FromXMMATRIX(rotYMat);
}
