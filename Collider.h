
#ifndef COLLIDER_H
#define COLLIDER_H

#include "btBulletDynamicsCommon.h"
#include <DirectXMath.h>
#include <memory>
#include "Component.h"
#include "PhysicsSystem.h"
#include "Quaternion.h"

// コライダーの種類
enum class ColliderType
{
	BOX,
	SPHERE,
	CAPSULE,
	CYLINDER
};

class PhysicsSystem;
class RigidBody;

// コライダー基底クラス
class Collider : public Component
{
private:

	// 所属するPhysicsSystem
	PhysicsSystem* m_PhysicsSystem = nullptr;

	// Bulletのコライダー形状
	std::unique_ptr<btCollisionShape> m_Shape = nullptr;

	// Bulletのコリジョンオブジェクト
	std::unique_ptr<btCollisionObject> m_CollisionObject = nullptr;

	// コライダーの種類
	ColliderType m_Type;

	// オフセット位置・回転
	DirectX::XMFLOAT3 m_OffsetPos{};
	Quaternion m_OffsetRot{};
	DirectX::XMFLOAT3 m_Scale{ 1.0f,1.0f,1.0f };

	// 静的コライダーかどうか
	bool m_IsStatic = true;

	// トリガー
	bool m_IsTrigger = false;

public:
	// コンストラクタ
	// isTrigger : 透過するかどうか
	// rigidbody : 所属する剛体（nullptrなら静的コライダー）
	// size : ボックスの場合は各辺の長さ、球の場合は半径、カプセルの場合は(半径、高さ) 
	// offsetPos : オフセット位置
	// offsetRot : オフセット回転（オイラー角）
	Collider(ColliderType type, DirectX::XMFLOAT3 size, bool isTrigger = false, DirectX::XMFLOAT3 offsetPos = {}, Quaternion offsetRot = {})
		: m_Type(type), m_IsTrigger(isTrigger), m_Scale(size), m_OffsetPos(offsetPos), m_OffsetRot(offsetRot)
	{}
	virtual ~Collider() { m_PhysicsSystem->UnregisterCollider(this); }

	// 衝突取得メソッド
	std::vector<GameObject*> GetCollisionEnter(GameObject* obj) { return m_PhysicsSystem->GetCollisionEnter(obj); }
	std::vector<GameObject*> GetCollisionStay(GameObject* obj) { return m_PhysicsSystem->GetCollisionStay(obj); }
	std::vector<GameObject*> GetCollisionExit(GameObject* obj) { return m_PhysicsSystem->GetCollisionExit(obj); }

	// トリガー取得メソッド
	std::vector<GameObject*> GetTriggerEnter(GameObject* obj) { return m_PhysicsSystem->GetTriggerEnter(obj); }
	std::vector<GameObject*> GetTriggerStay(GameObject* obj) { return m_PhysicsSystem->GetTriggerStay(obj); }
	std::vector<GameObject*> GetTriggerExit(GameObject* obj) { return m_PhysicsSystem->GetTriggerExit(obj); }

	friend class PhysicsSystem;
	friend class RigidBody;
};





#endif