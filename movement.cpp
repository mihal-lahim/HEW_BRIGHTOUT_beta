#include "Movement.h"
#include "Time.h"

using namespace DirectX;

void Movement::Start()
{
	// RigidBodyコンポーネントを取得
	m_RigidBody = gameObject()->GetComponent<RigidBody>();
}


Vector3 Movement::GetCurrentVelocity() const
{
	return m_RigidBody->GetVelocity();
}

void Movement::RotateByMoveVec()
{
	// XZ平面の回転を計算
	float yaw = atan2f(MoveVec.x, MoveVec.z);

	// オーナーの回転を更新
	gameObject()->transform.Rotation *= Quaternion::SetEulerY(yaw);
}

#include "debug_ostream.h"

void Movement::PostUpdate()
{
	// 力量を速度ベクトルに加算
	VelocityVec += ForceVec * (float)Time::DeltaTime();

	// インパルスを速度ベクトルに加算（反対方向の速度を打ち消す処理）
	if		(VelocityVec.x >= 0.0f) VelocityVec.x = ImpulseVec.x >= 0.0f ? VelocityVec.x + ImpulseVec.x : ImpulseVec.x;
	if		(VelocityVec.x <= 0.0f) VelocityVec.x = ImpulseVec.x <= 0.0f ? VelocityVec.x + ImpulseVec.x : ImpulseVec.x;

	if		(VelocityVec.y >= 0.0f) VelocityVec.y = ImpulseVec.y >= 0.0f ? VelocityVec.y + ImpulseVec.y : ImpulseVec.y;
	if		(VelocityVec.y <= 0.0f) VelocityVec.y = ImpulseVec.y <= 0.0f ? VelocityVec.y + ImpulseVec.y : ImpulseVec.y;

	if		(VelocityVec.z >= 0.0f) VelocityVec.z = ImpulseVec.z >= 0.0f ? VelocityVec.z + ImpulseVec.z : ImpulseVec.z;
	if		(VelocityVec.z <= 0.0f) VelocityVec.z = ImpulseVec.z <= 0.0f ? VelocityVec.z + ImpulseVec.z : ImpulseVec.z;

	// 移動ベクトルに速度ベクトルを加算して新しい速度を計算
	Vector3 newVelocity = VelocityVec + MoveVec;


	// RigidBodyがアクティブな場合
	if (m_RigidBody && m_RigidBody->IsActive())
	{
		// RigidBodyに速度を設定
		m_RigidBody->SetVelocity(newVelocity);
	}
	// RigidBodyがアクティブでない場合
	else
	{
		// 位置を更新
		gameObject()->transform.Position += newVelocity * (float)Time::DeltaTime();
	}

	// 力量ベクトルと移動ベクトルをリセット
	MoveVec = { 0.0f, 0.0f, 0.0f };
	ImpulseVec = { 0.0f, 0.0f, 0.0f };
	ForceVec = { 0.0f, 0.0f, 0.0f };
}