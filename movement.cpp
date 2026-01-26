#include "Movement.h"
#include "Time.h"

using namespace DirectX;

void Movement::Start()
{
	// RigidBodyコンポーネントを取得
	m_RigidBody = gameObject()->GetComponent<RigidBody>();
}

void Movement::AddMoveVec(const DirectX::XMFLOAT3& moveVec)
{
	XMVECTOR current = XMLoadFloat3(&m_MoveVec);
	XMVECTOR newMoveVec = XMVectorAdd(current, XMLoadFloat3(&moveVec));
	XMStoreFloat3(&m_MoveVec, newMoveVec);
}

void Movement::AddForceVec(const DirectX::XMFLOAT3& forceVec)
{
	XMVECTOR current = XMLoadFloat3(&m_ForceVec);
	XMVECTOR newForceVec = XMVectorAdd(current, XMLoadFloat3(&forceVec));
	XMStoreFloat3(&m_ForceVec, newForceVec);
}

void Movement::AddVelocityVec(const DirectX::XMFLOAT3& velocityVec)
{
	XMVECTOR current = XMLoadFloat3(&m_VelocityVec);
	XMVECTOR newVelocityVec = XMVectorAdd(current, XMLoadFloat3(&velocityVec));
	XMStoreFloat3(&m_VelocityVec, newVelocityVec);
}

void Movement::AddMoveVecV(const DirectX::XMVECTOR& moveVec)
{
	XMVECTOR current = XMLoadFloat3(&m_MoveVec);
	XMVECTOR newMoveVec = XMVectorAdd(current, moveVec);
	XMStoreFloat3(&m_MoveVec, newMoveVec);
}

void Movement::AddForceVecV(const DirectX::XMVECTOR& forceVec)
{
	XMVECTOR current = XMLoadFloat3(&m_ForceVec);
	XMVECTOR newForceVec = XMVectorAdd(current, forceVec);
	XMStoreFloat3(&m_ForceVec, newForceVec);
}

void Movement::AddVelocityVecV(const DirectX::XMVECTOR& velocityVec)
{
	XMVECTOR current = XMLoadFloat3(&m_VelocityVec);
	XMVECTOR newVelocityVec = XMVectorAdd(current, velocityVec);
	XMStoreFloat3(&m_VelocityVec, newVelocityVec);
}

DirectX::XMFLOAT3 Movement::GetCurrentVelocity() const
{
	return m_RigidBody->GetVelocity();
}


DirectX::XMVECTOR Movement::GetCurrentVelocityV() const
{
	XMFLOAT3 vel = m_RigidBody->GetVelocity();
	XMVECTOR velocity = XMLoadFloat3(&vel);
	return velocity;
}

void Movement::RotateByMoveVec()
{
	// 移動ベクトルを取得
	XMVECTOR vec = XMLoadFloat3(&m_MoveVec);

	// XZ平面の回転を計算
	float yaw = atan2f(-XMVectorGetX(vec), XMVectorGetZ(vec));

	// オーナーの回転を更新
	Quaternion rot{};
	gameObject()->transform.Rotation *= rot.SetEulerY(yaw);
}

void Movement::PostUpdate()
{
	// RigidBodyがアクティブな場合
	if (!m_RigidBody->IsActive())
	{
		// 力量ベクトルをRigidBodyに設定
		m_RigidBody->AddForce(m_ForceVec);

		// 速度ベクトルと移動ベクトルを合成
		XMVECTOR velocity = XMVectorAdd(XMLoadFloat3(&m_VelocityVec), XMLoadFloat3(&m_MoveVec));
		XMFLOAT3 newVelocity{};
		XMStoreFloat3(&newVelocity, velocity);

		//RigidBodyに設定
		m_RigidBody->SetVelocity(newVelocity);
	}
	// RigidBodyがアクティブでない場合
	else
	{
		// オーナーのTransformを直接更新
		Transform& transform = gameObject()->transform;

		// 位置ベクトルと移動ベクトルをXMVECTORに変換
		XMVECTOR position = XMLoadFloat3(&transform.Position);
		XMVECTOR moveVec = XMVectorAdd(XMLoadFloat3(&m_MoveVec), XMLoadFloat3(&m_VelocityVec));

		// 位置を更新
		position = XMVectorAdd(position, XMVectorScale(moveVec, (float)Time::DeltaTime()));

		// 更新した位置をTransformに保存
		XMStoreFloat3(&transform.Position, position);
	}

	// 力量ベクトルと移動ベクトルをリセット
	m_MoveVec = { 0.0f, 0.0f, 0.0f };
	m_ForceVec = { 0.0f, 0.0f, 0.0f };
}