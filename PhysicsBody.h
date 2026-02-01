
#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include "Component.h"
#include <vector>
#include <memory>
#include "btBulletDynamicsCommon.h"
#include "Vector3.h"
#include "ColliderShape.h"


// ボディタイプのenum
enum class BodyType
{
	STATIC,
	DYNAMIC,
	KINEMATIC
};

// 物理ボディ設定構造体
struct PhysicsBodyDesc
{
	BodyType Type = BodyType::STATIC;
	bool IsTrigger = false;
	float Mass = 1.0f;
	Vector3 Gravity{ 0.0f, -9.81f, 0.0f };
	Vector3 FixedRotation{ 0.0f, 0.0f, 0.0f };
};

class PhysicsBody : public Component
{
public:
	PhysicsBody(const PhysicsBodyDesc& desc)
		: m_type(desc.Type), m_isTrigger(desc.IsTrigger), m_mass(desc.Mass), m_gravity(desc.Gravity), m_fixedRotation(desc.FixedRotation)
	{}
	~PhysicsBody() = default;

	// 力を加えるメソッド
	void ApplyForce(const Vector3& force);

	// インパルスを加えるメソッド
	void ApplyImpulse(const Vector3& impulse);

	// 速度を設定するメソッド
	void SetVelocity(const Vector3& velocity);

	// 速度を取得するメソッド
	Vector3 GetVelocity() const;

	// コライダー形状を登録するメソッド
	void AddShape(const ColliderShape& colliderShape) { m_colliderShapes.push_back(colliderShape); }

	// オブジェクトのアクティブ状態を設定するメソッド
	void OnEnable() override;
	void OnDisable() override;

	// 衝突取得メソッド
	std::vector<PhysicsBody*> GetCollisionEnter();
	std::vector<PhysicsBody*> GetCollisionStay();
	std::vector<PhysicsBody*> GetCollisionExit();

	// トリガー取得メソッド
	std::vector<PhysicsBody*> GetTriggerEnter();
	std::vector<PhysicsBody*> GetTriggerStay();
	std::vector<PhysicsBody*> GetTriggerExit();

private:
	// ゲームコンテキストを使った初期化メソッド
	void InitializeByContext() override;
	void FinalizeByContext() override;

	// bullet側の実体
	std::unique_ptr<btCollisionObject> m_collisionObject = nullptr;

	// 剛体（DYNAMIC、KINEMATICの場合のみ有効）
	std::unique_ptr<btRigidBody> m_rigidBody = nullptr;

	// コライダー形状
	std::unique_ptr<btCollisionShape> m_collisionShape = nullptr;

	// モーションステート
	std::unique_ptr<btMotionState> m_motionState = nullptr;

	// ボディタイプ
	BodyType m_type = BodyType::STATIC;

	// トリガーかどうか
	bool m_isTrigger = false;

	// 質量（DYNAMICの場合のみ有効）
	float m_mass = 0.0f;

	// 重力（DYNAMICの場合のみ有効）
	Vector3 m_gravity{ 0.0f, -9.81f, 0.0f };

	// 固定される回転軸（1で固定、0で自由）
	Vector3 m_fixedRotation{ 0.0f, 0.0f, 0.0f };

	// 登録されたコライダー形状の配列
	std::vector<ColliderShape> m_colliderShapes{};

	friend class PhysicsSystem;
};


#endif
