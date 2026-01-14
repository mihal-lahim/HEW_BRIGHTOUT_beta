#include "movement.h"


using namespace DirectX;

void Movement::RotateByMoveVec()
{
	// 移動ベクトルを取得
	XMVECTOR vec = XMLoadFloat3(&m_MoveVec);

	// XZ平面の回転を計算
	float yaw = atan2f(-XMVectorGetX(vec), XMVectorGetZ(vec));

	// オーナーの回転を更新
	GetOwner()->SetRotation({0.0f, yaw, 0.0f});
}

void Movement::PostUpdate(double elapsedTime)
{
	// オーナーの位置を取得
	XMVECTOR pos = XMLoadFloat3(&owner->GetPosition());

	// 移動ベクトルと速度ベクトルを加算して新しい位置を計算
	XMVECTOR moveVec = XMLoadFloat3(&m_MoveVec) + XMLoadFloat3(&m_VelocityVec);
	moveVec = XMVectorScale(moveVec, static_cast<float>(elapsedTime));
	pos = XMVectorAdd(pos, moveVec);

	// オーナーの位置を更新
	XMFLOAT3 newPos{};
	XMStoreFloat3(&newPos, pos);
	owner->SetPosition(newPos);

	// 移動ベクトルをリセット
	m_MoveVec = { 0.0f, 0.0f, 0.0f };
}