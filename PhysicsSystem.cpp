#include "PhysicsSystem.h"
#include "PhysicsBody.h"
#include "ColliderShape.h"
#include "Ray.h"
#include "GameObject.h"
#include "Scene.h"
#include <algorithm>
#include <stdexcept>

using namespace DirectX;

btTransform PhysicsSystem::ApplyOffsets(const ColliderShape& colliderShape, Transform& tf, btCollisionShape*& shape)
{
	// 位置設定
	Vector3 pos = colliderShape.OffsetPos;

	// 回転設定
	Quaternion rot = colliderShape.OffsetRot;

	// トランスフォーム設定
	btTransform bttf{};
	bttf.setOrigin(ToBulletPosition(pos));
	bttf.setRotation(ToBulletRotation(rot));

	// サイズ設定
	Vector3 scale = colliderShape.Size;
	Vector3 ownerScale = tf.scale();

	scale.x *= ownerScale.x;
	scale.y *= ownerScale.y;
	scale.z *= ownerScale.z;

	btVector3 size = btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f);

	// コリジョン形状設定
	switch (colliderShape.Type)
	{
	case ShapeType::BOX:
		shape = new btBoxShape(size);
		break;
	case ShapeType::SPHERE:
		shape = new btSphereShape(size.x());
		break;
	case ShapeType::CAPSULE:
		shape = new btCapsuleShape(size.x(), size.y());
		break;
	case ShapeType::CYLINDER:
		shape = new btCylinderShape(size);
		break;
	}

	return bttf;
}

void PhysicsSystem::Initialize()
{
	m_broadphase = std::make_unique<btDbvtBroadphase>();
	m_collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfiguration.get());
	m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();

	m_dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
		m_dispatcher.get(),
		m_broadphase.get(),
		m_solver.get(),
		m_collisionConfiguration.get());

	m_dynamicsWorld->setGravity(btVector3(0.0f, 0.0f, 0.0f));
}

void PhysicsSystem::Finalize()
{
	m_dynamicsWorld.reset();
	m_solver.reset();
	m_dispatcher.reset();
	m_collisionConfiguration.reset();
	m_broadphase.reset();
}


void PhysicsSystem::RegisterPhysicsBody(PhysicsBody* physicsBody)
{
	// 剛体登録
	if (physicsBody->m_rigidBody)
	{
		m_dynamicsWorld->addRigidBody(physicsBody->m_rigidBody.get());
		physicsBody->m_rigidBody->setGravity(ToBulletPosition(physicsBody->m_gravity));
	}
	// コリジョンオブジェクト登録
	if (physicsBody->m_collisionObject)
	{
		m_dynamicsWorld->addCollisionObject(physicsBody->m_collisionObject.get());
	}
}

void PhysicsSystem::UnregisterPhysicsBody(PhysicsBody* physicsBody)
{
	// 剛体登録解除
	if (physicsBody->m_rigidBody)
	{
		m_dynamicsWorld->removeRigidBody(physicsBody->m_rigidBody.get());
	}
	// コリジョンオブジェクト登録解除
	if (physicsBody->m_collisionObject)
	{
		m_dynamicsWorld->removeCollisionObject(physicsBody->m_collisionObject.get());
	}
}



void PhysicsSystem::InitializePhysicsBody(PhysicsBody* physicsBody)
{
	// コライダー形状取得
	auto colliderShapes = physicsBody->m_colliderShapes;
	if (colliderShapes.empty())
	{
		throw std::runtime_error("PhysicsBody requires at least one ColliderShape.");
	}

	// コンパウンドシェイプ作成
	auto compoundShape = std::make_unique<btCompoundShape>();
	btCompoundShape* compoundShapePtr = compoundShape.get();

	float combinedFriction = 1.0f;
	float combinedRestitution = 0.0f;

	for (auto colliderShape : colliderShapes)
	{
		btCollisionShape* shape = nullptr;

		// オフセット適用
		btTransform bttf = ApplyOffsets(colliderShape, physicsBody->gameObject().transform(), shape);

		// コンパウンドシェイプに追加
		compoundShapePtr->addChildShape(bttf, shape);

		// マテリアル合成
		combinedFriction *= colliderShape.Friction;
		combinedRestitution = std::max(combinedRestitution, colliderShape.Restitution);
	}

	physicsBody->m_collisionShape = std::move(compoundShape);


	// 初期位置設定
	btTransform startPos;
	startPos.setOrigin(ToBulletPosition(physicsBody->gameObject().transform().position()));
	startPos.setRotation(ToBulletRotation(physicsBody->gameObject().transform().rotation()));

	// 静的ボディの場合はCollisionObjectとして登録
	if (physicsBody->m_type == BodyType::STATIC)
	{
		btCollisionObject* obj = new btCollisionObject();
		obj->setCollisionShape(physicsBody->m_collisionShape.get());
		obj->setWorldTransform(startPos);
		obj->setUserPointer(physicsBody);
		obj->setFriction(combinedFriction);
		obj->setRestitution(combinedRestitution);

		if (physicsBody->m_isTrigger)
			obj->setCollisionFlags(obj->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		physicsBody->m_collisionObject = std::unique_ptr<btCollisionObject>(obj);
		return;
	}

	float mass = (physicsBody->m_type == BodyType::DYNAMIC) ? physicsBody->m_mass : 0.0f;

	// 慣性テンソル
	btVector3 localInertia(0, 0, 0);
	if (mass > 0.0f)
		static_cast<btCompoundShape*>(physicsBody->m_collisionShape.get())->calculateLocalInertia(mass, localInertia);

	// 固定される回転軸の慣性テンソルを0に設定
	localInertia.setX(physicsBody->m_fixedRotation.x == 1.0f ? 0.0f : localInertia.x());
	localInertia.setY(physicsBody->m_fixedRotation.y == 1.0f ? 0.0f : localInertia.y());
	localInertia.setZ(physicsBody->m_fixedRotation.z == 1.0f ? 0.0f : localInertia.z());

	// モーションステート作成
	btDefaultMotionState* motionState = new btDefaultMotionState(startPos);

	// 剛体作成情報設定
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		mass,
		motionState,
		physicsBody->m_collisionShape.get(),
		localInertia);

	// 剛体作成
	btRigidBody* body = new btRigidBody(rbInfo);

	// ボディタイプ設定
	if (physicsBody->m_type == BodyType::KINEMATIC)
	{
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setActivationState(DISABLE_DEACTIVATION);
	}

	// トリガー設定
	if (physicsBody->m_isTrigger)
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	// ユーザーポインタ設定
	body->setUserPointer(physicsBody);

	// マテリアル設定
	body->setFriction(combinedFriction);
	body->setRestitution(combinedRestitution);

	// 重力の設定
	body->setGravity({ 0.0f,0.0f,0.0f });

	// 角度の固定設定
	btVector3 angularFactor = ToBulletPosition(physicsBody->m_fixedRotation);
	body->setAngularFactor(angularFactor);


	// メンバ変数に設定
	physicsBody->m_rigidBody = std::unique_ptr<btRigidBody>(body);
	physicsBody->m_motionState = std::unique_ptr<btMotionState>(motionState);
}



void PhysicsSystem::PhysicsUpdate(Scene& scene, float deltaTime)
{
	// 物理演算ステップ
	m_dynamicsWorld->stepSimulation(deltaTime);
	// シーン内の剛体取得
	std::vector<PhysicsBody*> physicsBodies = scene.GetComponents<PhysicsBody>();
	UpdatePhysicsBody(physicsBodies);
	UpdateCollisions();
}

void PhysicsSystem::UpdatePhysicsBody(std::vector<PhysicsBody*>& physicsBodies)
{
	for (auto* physicsBody : physicsBodies)
	{
		if (!physicsBody->IsEnable()) continue;
		if (!physicsBody->m_rigidBody) continue;

		if (physicsBody->m_type == BodyType::KINEMATIC)
		{
			// キネマティックボディの場合、TransformからBulletのトランスフォームを設定
			btTransform newTransform;
			newTransform.setOrigin(ToBulletPosition(physicsBody->gameObject().transform().position()));
			newTransform.setRotation(ToBulletRotation(physicsBody->gameObject().transform().rotation()));
			physicsBody->m_rigidBody->setWorldTransform(newTransform);
			continue;
		}

		// トランスフォーム取得
		btTransform worldTransform = physicsBody->m_rigidBody->getWorldTransform();

		// 位置更新
		Transform& tf = physicsBody->gameObject().transform();

		tf.position() = ToDirectXPosition(worldTransform.getOrigin());
		tf.rotation() = ToDirectXRotation(worldTransform.getRotation());

		// 力をリセット
		physicsBody->m_rigidBody->clearForces();
	}
}

void PhysicsSystem::UpdateCollisions()
{
	// 前回の衝突情報を保存
	m_previousCollisions = m_currentCollisions;

	// 衝突情報の取得
	int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();

	for (int i = 0; i < numManifolds; i++)
	{
		// 衝突マニホールド取得
		btPersistentManifold* contactManifold = m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		// 衝突オブジェクト取得
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		// 物理ボディ取得
		PhysicsBody* bodyA = static_cast<PhysicsBody*>(obA->getUserPointer());
		PhysicsBody* bodyB = static_cast<PhysicsBody*>(obB->getUserPointer());

		// 衝突点数取得
		int numContacts = contactManifold->getNumContacts();

		for (int j = 0; j < numContacts; j++)
		{
			// 衝突点取得
			btManifoldPoint& pt = contactManifold->getContactPoint(j);

			// 衝突しているか確認
			if (pt.getDistance() < 0.0f)
			{
				// トリガーかどうか確認
				bool isTriggerA = obA->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE;
				bool isTriggerB = obB->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE;

				// トリガーの場合
				if (isTriggerA || isTriggerB)
				{
					// トリガー情報に追加
					m_currentTriggers[bodyA].push_back(bodyB);
					m_currentTriggers[bodyB].push_back(bodyA);
				}
				// トリガーでない場合
				else
				{
					// 衝突情報に追加
					m_currentCollisions[bodyA].push_back(bodyB);
					m_currentCollisions[bodyB].push_back(bodyA);
				}
			}
		}
	}
}

void PhysicsSystem::RayCast(Ray& ray, float distance)
{
	// レイの方向を正規化して距離を掛ける
	Vector3 dir = ray.m_Direction;

	// ゼロベクトルならスキップ 
	if (dir.IsZero())
	{
		ray.Reset();
		return;
	}

	dir = dir.Normalize();
	dir = dir * distance;

	// レイの終点計算
	Vector3 to = ray.m_From + dir;


	// レイの始点と終点をBulletの形式に変換
	btVector3 from = ToBulletPosition(ray.m_From);
	btVector3 toVec = ToBulletPosition(to);

	// レイキャストの実行
	btCollisionWorld::ClosestRayResultCallback rayCallback(from, toVec);

	// レイテスト実行
	m_dynamicsWorld->rayTest(from, toVec, rayCallback);

	// レイがヒットしつつ、Triggerを無視する設定
	if (rayCallback.hasHit() && !(rayCallback.m_collisionObject->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE))
	{
		// ヒット情報をRayCastに設定
		ray.IsHit = true;
		ray.HitPosition = ToDirectXPosition(rayCallback.m_hitPointWorld);
		ray.HitDistance = (rayCallback.m_hitPointWorld - from).length();
		ray.HitNormal = ToDirectXPosition(rayCallback.m_hitNormalWorld);
		ray.HitObject = static_cast<PhysicsBody*>(rayCallback.m_collisionObject->getUserPointer());
		return;
	}
	else
	{
		// ヒットしなかった場合の初期化
		ray.Reset();
	}
}

std::vector<PhysicsBody*> PhysicsSystem::GetCollisionEnter(PhysicsBody* obj)
{
	// 衝突開始オブジェクトリスト
	std::vector<PhysicsBody*> enterObjects;

	for (auto* currentObj : m_currentCollisions[obj])
	{
		// 前回の衝突情報に存在しない場合、衝突開始
		auto it = std::find(m_previousCollisions[obj].begin(), m_previousCollisions[obj].end(), currentObj);

		if (it == m_previousCollisions[obj].end())
			enterObjects.push_back(currentObj);
	}

	return enterObjects;
}

std::vector<PhysicsBody*> PhysicsSystem::GetCollisionStay(PhysicsBody* obj)
{
	// 衝突継続オブジェクトリスト
	std::vector<PhysicsBody*> stayObjects;

	for (auto* currentObj : m_currentCollisions[obj])
	{
		// 前回の衝突情報に存在する場合、衝突継続
		auto it = std::find(m_previousCollisions[obj].begin(), m_previousCollisions[obj].end(), currentObj);

		if (it != m_previousCollisions[obj].end())
			stayObjects.push_back(currentObj);
	}

	return stayObjects;
}

std::vector<PhysicsBody*> PhysicsSystem::GetCollisionExit(PhysicsBody* obj)
{
	// 衝突終了オブジェクトリスト
	std::vector<PhysicsBody*> exitObjects;

	for (auto* previousObj : m_previousCollisions[obj])
	{
		// 現在の衝突情報に存在しない場合、衝突終了
		auto it = std::find(m_currentCollisions[obj].begin(), m_currentCollisions[obj].end(), previousObj);

		if (it == m_currentCollisions[obj].end())
			exitObjects.push_back(previousObj);
	}

	return exitObjects;
}

std::vector<PhysicsBody*> PhysicsSystem::GetTriggerEnter(PhysicsBody* obj)
{
	// トリガー開始オブジェクトリスト
	std::vector<PhysicsBody*> enterObjects;

	for (auto* currentObj : m_currentTriggers[obj])
	{
		// 前回のトリガー情報に存在しない場合、トリガー開始
		auto it = std::find(m_previousTriggers[obj].begin(), m_previousTriggers[obj].end(), currentObj);

		if (it == m_previousTriggers[obj].end())
			enterObjects.push_back(currentObj);
	}

	return enterObjects;
}

std::vector<PhysicsBody*> PhysicsSystem::GetTriggerStay(PhysicsBody* obj)
{
	// トリガー継続オブジェクトリスト
	std::vector<PhysicsBody*> stayObjects;

	for (auto* currentObj : m_currentTriggers[obj])
	{
		// 前回のトリガー情報に存在する場合、トリガー継続
		auto it = std::find(m_previousTriggers[obj].begin(), m_previousTriggers[obj].end(), currentObj);

		if (it != m_previousTriggers[obj].end())
			stayObjects.push_back(currentObj);
	}

	return stayObjects;
}

std::vector<PhysicsBody*> PhysicsSystem::GetTriggerExit(PhysicsBody* obj)
{
	// トリガー終了オブジェクトリスト
	std::vector<PhysicsBody*> exitObjects;

	for (auto* previousObj : m_previousTriggers[obj])
	{
		// 現在のトリガー情報に存在しない場合、トリガー終了
		auto it = std::find(m_currentTriggers[obj].begin(), m_currentTriggers[obj].end(), previousObj);

		if (it == m_currentTriggers[obj].end())
			exitObjects.push_back(previousObj);
	}

	return exitObjects;
}
