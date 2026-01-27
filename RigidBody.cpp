#include "RigidBody.h"
#include "PhysicsSystem.h"
#include "Collider.h"
#include "GameObject.h"

using namespace DirectX;

void RigidBody::AddCollider(Collider* collider)
{
	collider->m_IsStatic = false;
	m_Colliders.push_back(collider);
}

void RigidBody::AddForce(XMFLOAT3 force)
{
	// 剛体をアクティブ化
	m_RigidBody->activate(true);

	// Bullet座標系に変換
	btVector3 btForce = ToBulletPosition(force);

	// 力を加える
	m_RigidBody->applyCentralForce(btForce);
}

void RigidBody::SetVelocity(XMFLOAT3 velocity)
{
	// 剛体をアクティブ化
	m_RigidBody->activate(true);

	// Bullet座標系に変換
	btVector3 btVelocity = ToBulletPosition(velocity);

	// 速度を設定
	m_RigidBody->setLinearVelocity(btVelocity);
}

DirectX::XMFLOAT3 RigidBody::GetVelocity()
{
	// 速度取得
	btVector3 btVelocity = m_RigidBody->getLinearVelocity();

	// DirectX座標系に変換
	XMFLOAT3 velocity = ToDirectXPosition(btVelocity);

	return velocity;
}

void RigidBody::SetGravity(DirectX::XMFLOAT3 gravity)
{
	// Bullet座標系に変換
	btVector3 btGravity = ToBulletPosition(gravity);

	// メンバ変数更新
	m_Gravity = gravity;

	// 重力を設定
	if(m_RigidBody)
	m_RigidBody->setGravity(btGravity);
}

void RigidBody::SetActive(bool isActive)
{
	// アクティブ化される場合
	if (isActive && !IsActive())
	{
		m_PhysicsSystem->RegisterRigidBody(this);
	}
	// 剛体を非アクティブ化
	else if(!isActive && IsActive())
	{
		m_PhysicsSystem->UnregisterRigidBody(this);
	}

	Component::SetActive(isActive);
}
