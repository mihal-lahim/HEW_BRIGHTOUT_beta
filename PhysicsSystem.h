
#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <vector>
#include <DirectXMath.h>
#include <unordered_map>
#include "btBulletDynamicsCommon.h"
#include "Transform.h"

class Collider;
class RigidBody;
class GameObject;
class RayCast;

class PhysicsSystem
{
private:
	// Bulletの物理演算ワールド
	btDiscreteDynamicsWorld* m_DynamicsWorld = nullptr;

	// 衝突マップ（現在）
	std::unordered_map<GameObject*, std::vector<GameObject*>> m_CurrentCollisions{};
	// 衝突マップ（前回）
	std::unordered_map<GameObject*, std::vector<GameObject*>> m_PreviousCollisions{};

	// トリガーマップ（現在）
	std::unordered_map<GameObject*, std::vector<GameObject*>> m_CurrentTriggers{};
	// トリガーマップ（前回）
	std::unordered_map<GameObject*, std::vector<GameObject*>> m_PreviousTriggers{};

	// コライダーのオフセット適用
	const btTransform& ApplyOffsets(Collider& collider);
public:

	PhysicsSystem();

	// コライダー登録
	void RegisterColliders(Collider* collider);
	// 剛体登録
	void RegisterRigidBodies(RigidBody* rigidbody);

	// コライダー登録解除
	void UnregisterCollider(Collider* collider);
	// 剛体登録解除
	void UnregisterRigidBody(RigidBody* rigidbody);

	// 衝突取得メソッド
	std::vector<GameObject*> GetCollisionEnter(GameObject* obj);
	std::vector<GameObject*> GetCollisionStay(GameObject* obj);
	std::vector<GameObject*> GetCollisionExit(GameObject* obj);

	// トリガー取得メソッド
	std::vector<GameObject*> GetTriggerEnter(GameObject* obj);
	std::vector<GameObject*> GetTriggerStay(GameObject* obj);
	std::vector<GameObject*> GetTriggerExit(GameObject* obj);

	// 更新
	void PhysicsUpdate(float deltaTime);
	void UpdateRigidBody(std::vector<RigidBody*>& rigidbodies);
	void UpdateCollisions();
	void UpdateRayCasts(std::vector<RayCast*>& raycasts);
};

// DirectX座標系からBullet座標系への変換
btVector3 ToBulletPosition(const DirectX::XMFLOAT3 pos) { return btVector3(pos.x, pos.y, -pos.z); }

// Bullet座標系からDirectX座標系への変換
DirectX::XMFLOAT3 ToDirectXPosition(const btVector3& vec) { return DirectX::XMFLOAT3(vec.x(), vec.y(), -vec.z()); }

// DirectX回転からBullet回転への変換
const btQuaternion& ToBulletRotation(const Quaternion& rot) { return btQuaternion(rot.Quat.x, rot.Quat.y, -rot.Quat.z, rot.Quat.w); }

// Bullet回転（クォータニオン）からDirectX回転（オイラー角）への変換
const Quaternion& ToDirectXRotation(const btQuaternion& quat) { return Quaternion{ DirectX::XMFLOAT4(quat.x(), quat.y(), -quat.z(), quat.w()) }; }

#endif