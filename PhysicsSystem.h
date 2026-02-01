#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <vector>
#include <unordered_map>
#include "btBulletDynamicsCommon.h"
#include "Transform.h"
#include "Vector3.h"
#include <memory>
#include "UniqueQueue.h"


struct ColliderShape;
class PhysicsBody;
struct Ray;
class Scene;
class EngineCore;

class PhysicsSystem
{
public:
	// レイキャスト
	void RayCast(Ray& ray, float distance);


	PhysicsSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{ 
		Initialize();
	}
	~PhysicsSystem() 
	{ 
		Finalize();
	}

	void Initialize();
	void Finalize();


	// 物理ボディ初期化
	void InitializePhysicsBody(PhysicsBody* physicsBody);

	// 物理ボディ登録
	void RegisterPhysicsBody(PhysicsBody* physicsBody);
	// 物理ボディ登録解除
	void UnregisterPhysicsBody(PhysicsBody* physicsBody);


	// 衝突取得メソッド
	std::vector<PhysicsBody*> GetCollisionEnter(PhysicsBody* obj);
	std::vector<PhysicsBody*> GetCollisionStay(PhysicsBody* obj);
	std::vector<PhysicsBody*> GetCollisionExit(PhysicsBody* obj);

	// トリガー取得メソッド
	std::vector<PhysicsBody*> GetTriggerEnter(PhysicsBody* obj);
	std::vector<PhysicsBody*> GetTriggerStay(PhysicsBody* obj);
	std::vector<PhysicsBody*> GetTriggerExit(PhysicsBody* obj);

	// 更新
	void PhysicsUpdate(Scene& scene, float deltaTime);


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
	std::unordered_map<PhysicsBody*, std::vector<PhysicsBody*>> m_currentCollisions{};
	// 衝突マップ（前回）
	std::unordered_map<PhysicsBody*, std::vector<PhysicsBody*>> m_previousCollisions{};

	// トリガーマップ（現在）
	std::unordered_map<PhysicsBody*, std::vector<PhysicsBody*>> m_currentTriggers{};
	// トリガーマップ（前回）
	std::unordered_map<PhysicsBody*, std::vector<PhysicsBody*>> m_previousTriggers{};




	// コライダーのオフセット適用
	btTransform ApplyOffsets(const ColliderShape& colliderShape, Transform& tf, btCollisionShape*& shape);

	void UpdatePhysicsBody(std::vector<PhysicsBody*>& physicsBodies);
	void UpdateCollisions();
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