#include "RigidBody.h"
#include "PhysicsSystem.h"
#include "Collider.h"

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

const DirectX::XMFLOAT3& RigidBody::GetVelocity()
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

void RigidBody::SetDeActivate()
{
	m_IsDeActivated = true;
	m_PhysicsSystem->UnregisterRigidBody(this);
}

void RigidBody::ReturnActivate(const Transform& returnTransform)
{
	m_IsDeActivated = false;
	m_PhysicsSystem->RegisterRigidBodies(this);
	// 位置を設定
	btTransform transform;
	transform.setOrigin(ToBulletPosition(returnTransform.Position));
	// 回転を設定
	transform.setRotation(ToBulletRotation(returnTransform.Rotation));

	// 剛体のワールド変換を更新
	m_RigidBody->setWorldTransform(transform);
}
