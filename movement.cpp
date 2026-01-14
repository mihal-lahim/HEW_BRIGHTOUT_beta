#include "movement.h"

using namespace DirectX;

void Movement::Start()
{
	// RigidBodyコンポーネントを取得
	m_RigidBody = GetOwner()->GetComponent<RigidBody>();
}

void Movement::RotateByMoveVec()
{
	// 移動ベクトルを取得
	XMVECTOR vec = XMLoadFloat3(&m_MoveVec);

	// XZ平面の回転を計算
	float yaw = atan2f(-XMVectorGetX(vec), XMVectorGetZ(vec));

	// オーナーの回転を更新
	Quaternion rot{};
	GetOwner()->Transform.Rotation *= rot.SetEulerY(yaw);
}

void Movement::PostUpdate(double elapsedTime)
{
	// RigidBodyがアクティブな場合
	if (!m_RigidBody->GetDeActivateFlag())
	{
		// 現在の速度を取得
		XMVECTOR current = XMLoadFloat3(&m_RigidBody->GetVelocity());
		// 移動ベクトルをXMVECTORに変換
		XMVECTOR moveVec = XMLoadFloat3(&m_MoveVec);

		// 新しい速度を計算
		XMVECTOR newVelocity = XMVectorAdd(current, moveVec);

		XMFLOAT3 velocity{};
		// 新しい速度をXMFLOAT3に変換して保存
		XMStoreFloat3(&velocity, newVelocity);

		//移動ベクトルをRididBodyに設定
		m_RigidBody->SetVelocity(velocity);

		// 力量ベクトルRigidBodyに適用
		m_RigidBody->AddForce(m_ForceVec);


		// 力量ベクトルと移動ベクトルをリセット
		m_MoveVec = { 0.0f, 0.0f, 0.0f };
		m_ForceVec = { 0.0f, 0.0f, 0.0f };
	}
	// RigidBodyがアクティブでない場合
	else
	{
		// オーナーのTransformを直接更新
		Transform& transform = GetOwner()->Transform;

		// 位置ベクトルと移動ベクトルをXMVECTORに変換
		XMVECTOR position = XMLoadFloat3(&transform.Position);
		XMVECTOR moveVec = XMLoadFloat3(&m_MoveVec);

		// 位置を更新
		position = XMVectorAdd(position, XMVectorScale(moveVec, static_cast<float>(elapsedTime)));

		// 更新した位置をTransformに保存
		XMStoreFloat3(&transform.Position, position);
	}
}