

#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <vector>
#include <unordered_map>
#include "btBulletDynamicsCommon.h"
#include "Transform.h"
#include "Vector3.h"
#include <memory>


class Collider;
class RigidBody;
class GameObject;
class Ray;
class Scene;
class EngineCore;

class PhysicsSystem
{
private:
	// 所属するエンジンコア
	EngineCore* m_engineCore = nullptr;

	// Bulletの物理演算ワールド
	std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld = nullptr;

	// ブロードフェーズインターフェース
	std::unique_ptr<btBroadphaseInterface> m_broadphase = nullptr;
	// 衝突設定
	std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration = nullptr;
	// 衝突ディスパッチャー
	std::unique_ptr<btCollisionDispatcher> m_dispatcher = nullptr;
	// 制約ソルバー
	std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver = nullptr;

	// 衝突マップ（現在）
	std::unordered_map<GameObject*, std::vector<GameObject*>> m_currentCollisions{};
	// 衝突マップ（前回）
	std::unordered_map<GameObject*, std::vector<GameObject*>> m_previousCollisions{};

	// トリガーマップ（現在）
	std::unordered_map<GameObject*, std::vector<GameObject*>> m_currentTriggers{};
	// トリガーマップ（前回）
	std::unordered_map<GameObject*, std::vector<GameObject*>> m_previousTriggers{};


	// コライダーのオフセット適用
	btTransform ApplyOffsets(Collider& collider);

	void UpdateRigidBody(std::vector<RigidBody*>& rigidbodies);
	void UpdateCollisions();
public:

	PhysicsSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{ Initialize(); }
	~PhysicsSystem() { Finalize(); }

	void Initialize();
	void Finalize();

	// コライダー登録
	void RegisterCollider(Collider* collider);
	// 剛体登録
	void RegisterRigidBody(RigidBody* rigidbody);

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
	void PhysicsUpdate(Scene& scene, float deltaTime);

	// レイキャスト
	void RayCast(Ray& ray, float distance);
};

// DirectX座標系からBullet座標系への変換
inline btVector3 ToBulletPosition(const Vector3& pos) { return btVector3(pos.x, pos.y, -pos.z); }

// Bullet座標系からDirectX座標系への変換
inline Vector3 ToDirectXPosition(const btVector3& vec) { return Vector3(vec.x(), vec.y(), -vec.z()); }

// DirectX回転からBullet回転への変換
inline btQuaternion ToBulletRotation(const Quaternion& rot) { return btQuaternion(rot.Quat.x, rot.Quat.y, -rot.Quat.z, rot.Quat.w); }

// Bullet回転（クォータニオン）からDirectX回転（オイラー角）への変換
inline Quaternion ToDirectXRotation(const btQuaternion& quat) { return Quaternion{ Vector4(quat.x(), quat.y(), -quat.z(), quat.w()) }; }

#endif