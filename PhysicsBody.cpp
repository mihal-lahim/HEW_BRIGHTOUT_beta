#include "PhysicsBody.h"
#include "PhysicsSystem.h"

void PhysicsBody::ApplyForce(const Vector3& force)
{
	if (!m_rigidBody) return;

	// 剛体をアクティブ化
	m_rigidBody->activate(true);
	// Bullet座標系に変換
	btVector3 btForce(force.x, force.y, force.z);
	// 力を加える
	m_rigidBody->applyCentralForce(btForce);
}

void PhysicsBody::ApplyImpulse(const Vector3& impulse)
{
	if (!m_rigidBody) return;

	// 剛体をアクティブ化
	m_rigidBody->activate(true);
	// Bullet座標系に変換
	btVector3 btImpulse(impulse.x, impulse.y, impulse.z);
	// インパルスを加える
	m_rigidBody->applyCentralImpulse(btImpulse);
}

void PhysicsBody::SetVelocity(const Vector3& velocity)
{
	if (!m_rigidBody) return;

	// 剛体をアクティブ化
	m_rigidBody->activate(true);
	// Bullet座標系に変換
	btVector3 btVelocity(velocity.x, velocity.y, velocity.z);
	// 速度を設定
	m_rigidBody->setLinearVelocity(btVelocity);
}

Vector3 PhysicsBody::GetVelocity() const
{
	if (!m_rigidBody) return {};

	// 速度取得
	btVector3 btVelocity = m_rigidBody->getLinearVelocity();
	// 独自座標系に変換して返す
	return Vector3(btVelocity.getX(), btVelocity.getY(), btVelocity.getZ());
}


void PhysicsBody::OnEnable()
{
	physics().RegisterPhysicsBody(this);
}

void PhysicsBody::OnDisable()
{
	physics().UnregisterPhysicsBody(this);
}

std::vector<PhysicsBody*> PhysicsBody::GetCollisionEnter()
{
	return physics().GetCollisionEnter(this);
}

std::vector<PhysicsBody*> PhysicsBody::GetCollisionStay()
{
	return physics().GetCollisionStay(this);
}

std::vector<PhysicsBody*> PhysicsBody::GetCollisionExit()
{
	return physics().GetCollisionExit(this);
}

std::vector<PhysicsBody*> PhysicsBody::GetTriggerEnter()
{
	return physics().GetTriggerEnter(this);
}

std::vector<PhysicsBody*> PhysicsBody::GetTriggerStay()
{
	return physics().GetTriggerStay(this);
}

std::vector<PhysicsBody*> PhysicsBody::GetTriggerExit()
{
	return physics().GetTriggerExit(this);
}


void PhysicsBody::InitializeByContext()
{
	physics().InitializePhysicsBody(this);
	physics().RegisterPhysicsBody(this);
}

void PhysicsBody::FinalizeByContext()
{
	physics().UnregisterPhysicsBody(this);
}
