#include "Movement.h"
#include "Time.h"

using namespace DirectX;

void Movement::Start()
{
	// RigidBodyコンポーネントを取得
	m_physicsBody = gameObject().GetComponent<PhysicsBody>();
}


Vector3 Movement::GetCurrentVelocity() const
{
	return m_physicsBody->GetVelocity();
}

void Movement::RotateByMoveVec()
{
	// 移動ベクトルがゼロの場合は回転しない
	if (MoveVec.x == 0.0f && MoveVec.z == 0.0f)
		return;

	// XZ平面の回転を計算
	float yaw = XMConvertToDegrees(atan2f(MoveVec.x, MoveVec.z));

	// 移動方向に向きを設定
	gameObject().transform().rotation() = Quaternion::SetEulerY(yaw);
}


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
	if (m_physicsBody && m_physicsBody->IsEnable())
	{
		// RigidBodyに速度を設定
		m_physicsBody->SetVelocity(newVelocity);
	}
	// RigidBodyがアクティブでない場合
	else
	{
		// 位置を更新
		gameObject().transform().position() += newVelocity * (float)Time::DeltaTime();
	}

	// 力量ベクトルと移動ベクトルをリセット
	MoveVec = { 0.0f, 0.0f, 0.0f };
	ImpulseVec = { 0.0f, 0.0f, 0.0f };
	ForceVec = { 0.0f, 0.0f, 0.0f };
}