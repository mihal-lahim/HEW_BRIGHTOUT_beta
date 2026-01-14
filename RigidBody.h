
#ifndef RIGIDBODY_H
#define RIGIDBODY_H


#include "btBulletDynamicsCommon.h"
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "Component.h"
#include "PhysicsSystem.h"
#include "Transform.h"

class Collider;
class PhysicsSystem;

class RigidBody : public Component
{
private:

	// 所属するPhysicsSystem
	PhysicsSystem* m_PhysicsSystem = nullptr;

	// 剛体
	std::unique_ptr<btRigidBody> m_RigidBody = nullptr;
	// モーションステート
	std::unique_ptr<btMotionState> m_MotionState = nullptr;
	// コンパウンドシェイプ
	std::unique_ptr<btCompoundShape> m_CompoundShape = nullptr;

	// 所属するコライダー群
	std::vector<Collider*> m_Colliders = {};

	// 質量
	float m_Mass = 1.0f;
	// 重力加速度
	DirectX::XMFLOAT3 m_Gravity = { 0.0f, -9.8f, 0.0f };
	// 固定される回転軸（1で固定、0で自由）
	DirectX::XMFLOAT3 m_FixedRotation = {};

	// トリガー
	bool m_IsTrigger = false;

	// 一時的に非アクティブ化されているかどうか
	bool m_IsDeActivated = false;
public:

	// コンストラクタ
	RigidBody(float mass = 1.0f, DirectX::XMFLOAT3 fixedRotation = {}, bool isTrigger = false)
		: m_Mass(mass), m_FixedRotation(fixedRotation), m_IsTrigger(isTrigger)
	{}
	virtual ~RigidBody() { m_PhysicsSystem->UnregisterRigidBody(this); }

	// コライダー追加メソッド
	void AddCollider(Collider* collider);

	// 力を加えるメソッド
	void AddForce(DirectX::XMFLOAT3 force);
	// 速度を設定するメソッド
	void SetVelocity(DirectX::XMFLOAT3 velocity);

	// 速度を取得するメソッド
	const DirectX::XMFLOAT3& GetVelocity();

	// 重力を設定するメソッド
	void SetGravity(DirectX::XMFLOAT3 gravity);


	// 一時的に非アクティブ化するメソッド
	void SetDeActivate();

	// 再アクティブ化するメソッド
	void ReturnActivate(const Transform& returnTransform);

	// 非アクティブ化フラグ取得メソッド
	bool GetDeActivateFlag() { return m_IsDeActivated; }

	friend PhysicsSystem;
};


#endif
