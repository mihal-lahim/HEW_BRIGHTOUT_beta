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

void Movement::PostUpdate()
{
	// RigidBodyがアクティブな場合
	if (m_RigidBody->IsActive())
	{
		//RigidBodyに設定
		m_RigidBody->SetVelocity(MoveVec + VelocityVec);

		// インパルスベクトルをRigidBodyに設定
		m_RigidBody->AddImpulse(ImpulseVec);

		// 力量ベクトルをRigidBodyに設定
		m_RigidBody->AddForce(ForceVec);
	}
	// RigidBodyがアクティブでない場合
	else
	{
		// 速度ベクトルを更新
		VelocityVec += ForceVec * (float)Time::DeltaTime() + ImpulseVec;

		// 位置を更新
		gameObject()->transform.Position += (MoveVec + VelocityVec) * (float)Time::DeltaTime();
	}

	// 力量ベクトルと移動ベクトルをリセット
	MoveVec = { 0.0f, 0.0f, 0.0f };
	ImpulseVec = { 0.0f, 0.0f, 0.0f };
	ForceVec = { 0.0f, 0.0f, 0.0f };
}