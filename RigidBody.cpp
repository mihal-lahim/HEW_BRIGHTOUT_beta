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

void RigidBody::AddForce(Vector3 force)
{
	// 剛体をアクティブ化
	m_RigidBody->activate(true);

	// Bullet座標系に変換
	btVector3 btForce = ToBulletPosition(force);

	// 力を加える
	m_RigidBody->applyCentralForce(btForce);
}

void RigidBody::AddImpulse(Vector3 impulse)
{
	// 剛体をアクティブ化
	m_RigidBody->activate(true);

	// Bullet座標系に変換
	btVector3 btImpulse = ToBulletPosition(impulse);

	// インパルスを加える
	m_RigidBody->applyCentralImpulse(btImpulse);
}

void RigidBody::SetVelocity(Vector3 velocity)
{
	// 剛体をアクティブ化
	m_RigidBody->activate(true);

	// Bullet座標系に変換
	btVector3 btVelocity = ToBulletPosition(velocity);

	// 速度を設定
	m_RigidBody->setLinearVelocity(btVelocity);
}

Vector3 RigidBody::GetVelocity()
{
	// 速度取得
	btVector3 btVelocity = m_RigidBody->getLinearVelocity();

	// DirectX座標系に変換
	Vector3 velocity = ToDirectXPosition(btVelocity);

	return velocity;
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
