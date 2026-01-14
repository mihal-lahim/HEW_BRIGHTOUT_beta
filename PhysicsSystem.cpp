
#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "Collider.h"
#include "RayCast.h"
#include "GameObject.h"


using namespace DirectX;

const btTransform& PhysicsSystem::ApplyOffsets(Collider& collider)
{
	// 所有者のゲームオブジェクトのTransform取得
	Transform* tf = &collider.GetOwner()->Transform;

	// 位置設定
	XMVECTOR pos = XMVectorAdd(XMLoadFloat3(&tf->Position), XMLoadFloat3(&collider.m_OffsetPos));
	XMFLOAT3 btPos{};
	XMStoreFloat3(&btPos, pos);

	// 回転設定
	Quaternion ownerRot = tf->Rotation;
	Quaternion offsetRot = collider.m_OffsetRot;

	XMFLOAT4 combinedRot = (ownerRot * offsetRot).Quat;

	// トランスフォーム設定
	btTransform bttf{};
	bttf.setOrigin(ToBulletPosition(btPos));
	bttf.setRotation(ToBulletRotation(Quaternion{ combinedRot }));


	// サイズ設定
	XMFLOAT3 scale = collider.m_Scale;
	XMFLOAT3 ownerScale = tf->Scale;

	scale.x *= ownerScale.x;
	scale.y *= ownerScale.y;
	scale.z *= ownerScale.z;

	btVector3 size = btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f);

	// コリジョン形状設定
	switch (collider.m_Type)
	{
	case ColliderType::BOX:
		collider.m_Shape = std::make_unique<btBoxShape>(size);
		break;
	case ColliderType::SPHERE:
		collider.m_Shape = std::make_unique<btSphereShape>(size);
		break;
	case ColliderType::CAPSULE:
		collider.m_Shape = std::make_unique<btCapsuleShape>(size);
		break;
	case ColliderType::CYLINDER:
		collider.m_Shape = std::make_unique<btCylinderShape>(size);
		break;
	}

	return bttf;
}

PhysicsSystem::PhysicsSystem()
{
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	m_DynamicsWorld->setGravity(btVector3(0, 0, 0));
	m_DynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
}

void PhysicsSystem::RegisterColliders(Collider* collider)
{
	// 所属するPhysicsSystem設定
	collider->m_PhysicsSystem = this;

	// オフセット適用
	btTransform bttf = ApplyOffsets(*collider);

	// 静的コライダーの場合スキップ
	if (!collider->m_IsStatic) return;

	// コリジョンオブジェクト作成
	btCollisionObject* obj = new btCollisionObject();

	// コリジョン形状設定
	obj->setCollisionShape(collider->m_Shape.get());

	// トランスフォーム設定
	obj->setWorldTransform(bttf);

	// ユーザーポインタ設定
	obj->setUserPointer(collider->GetOwner());

	// トリガー設定
	if (collider->m_IsTrigger)
		obj->setCollisionFlags(obj->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	// コライダー登録
	m_DynamicsWorld->addCollisionObject(obj);

	collider->m_CollisionObject = std::unique_ptr<btCollisionObject>(obj);
}

void PhysicsSystem::RegisterRigidBodies(RigidBody* rigidbody)
{
	// 所属するPhysicsSystem設定
	rigidbody->m_PhysicsSystem = this;

	// コンパウンドシェイプ作成
	rigidbody->m_CompoundShape = std::make_unique<btCompoundShape>();
	btCompoundShape* compoundShape = rigidbody->m_CompoundShape.get();

	for (auto* collider : rigidbody->m_Colliders)
	{
		// オフセット適用
		btTransform bttf = ApplyOffsets(*collider);

		// コンパウンドシェイプに追加
		compoundShape->addChildShape(bttf, collider->m_Shape.get());
	}


	// 慣性テンソル
	btVector3 localInertia(0, 0, 0);

	// 質量がある場合は慣性テンソルを計算
	if (rigidbody->m_Mass > 0.0f)
		compoundShape->calculateLocalInertia(rigidbody->m_Mass, localInertia);

	// 初期位置設定
	btTransform startPos;
	startPos.setOrigin(ToBulletPosition(rigidbody->GetOwner()->Transform.Position));
	startPos.setRotation(ToBulletRotation(rigidbody->GetOwner()->Transform.Rotation));

	// モーションステート作成
	btDefaultMotionState* motionState = new btDefaultMotionState(startPos);

	// 剛体作成情報設定
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		rigidbody->m_Mass,
		motionState,
		compoundShape,
		localInertia);

	// 剛体作成
	btRigidBody* body = new btRigidBody(rbInfo);

	// XMFLOAT3をbtVector3に変換
	btVector3 gravity = ToBulletPosition(rigidbody->m_Gravity);

	// 重力の設定
	body->setGravity(gravity);

	// XMFLOAT3をbtVector3に変換
	btVector3 angularFactor = ToBulletPosition(rigidbody->m_FixedRotation);

	// 角度の固定設定
	rigidbody->m_RigidBody->setAngularFactor(angularFactor);

	// トリガー設定
	if (rigidbody->m_IsTrigger)
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	// ユーザーポインタ設定
	body->setUserPointer(rigidbody->GetOwner());

	// 剛体登録
	m_DynamicsWorld->addRigidBody(body);

	rigidbody->m_RigidBody = std::unique_ptr<btRigidBody>(body);
	rigidbody->m_MotionState = std::unique_ptr<btMotionState>(motionState);
}

void PhysicsSystem::UnregisterCollider(Collider* collider)
{
	m_DynamicsWorld->removeCollisionObject(collider->m_CollisionObject.get());
}

void PhysicsSystem::UnregisterRigidBody(RigidBody* rigidbody)
{
	m_DynamicsWorld->removeRigidBody(rigidbody->m_RigidBody.get());
}


void PhysicsSystem::PhysicsUpdate(float deltaTime)
{
	// 物理演算ステップ
	m_DynamicsWorld->stepSimulation(deltaTime);
}


void PhysicsSystem::UpdateRigidBody(std::vector<RigidBody*>& rigidbodies)
{
	for (auto* rigidbody : rigidbodies)
	{
		// トランスフォーム取得
		btTransform worldTransform = rigidbody->m_RigidBody->getWorldTransform();

		// 位置更新
		Transform* tf = &rigidbody->GetOwner()->Transform;

		tf->Position = ToDirectXPosition(worldTransform.getOrigin());
		tf->Rotation = ToDirectXRotation(worldTransform.getRotation());

		// 力をリセット
		rigidbody->m_RigidBody->clearForces();
	}
}

void PhysicsSystem::UpdateCollisions()
{
	// 前回の衝突情報を保存
	m_PreviousCollisions = m_CurrentCollisions;

	// 衝突情報の取得
	int numManifolds = m_DynamicsWorld->getDispatcher()->getNumManifolds();

	for (int i = 0; i < numManifolds; i++)
	{
		// 衝突マニホールド取得
		btPersistentManifold* contactManifold = m_DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		// 衝突オブジェクト取得
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		// ゲームオブジェクト取得
		GameObject* gameObjectA = static_cast<GameObject*>(obA->getUserPointer());
		GameObject* gameObjectB = static_cast<GameObject*>(obB->getUserPointer());

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
					m_CurrentTriggers[gameObjectA].push_back(gameObjectB);
					m_CurrentTriggers[gameObjectB].push_back(gameObjectA);
				}
				// トリガーでない場合
				else
				{
					// 衝突情報に追加
					m_CurrentCollisions[gameObjectA].push_back(gameObjectB);
					m_CurrentCollisions[gameObjectB].push_back(gameObjectA);
				}
			}
		}
	}
}

void PhysicsSystem::UpdateRayCasts(std::vector<RayCast*>& raycasts)
{
	for (auto* raycast : raycasts)
	{
		// レイの始点と終点をBulletの形式に変換
		btVector3 from = ToBulletPosition(raycast->m_From);
		btVector3 to = ToBulletPosition(raycast->m_To);

		// レイキャストの実行
		btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);

		// レイがヒットしつつ、Triggerを無視する設定
		if (rayCallback.hasHit() && !(rayCallback.m_collisionObject->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE))
		{
			// ヒット情報をRayCastに設定
			raycast->m_IsHit = true;
			raycast->m_HitPosition = ToDirectXPosition(rayCallback.m_hitPointWorld);
			raycast->m_HitDistance = (rayCallback.m_hitPointWorld - from).length();
			raycast->m_HitNormal = ToDirectXPosition(rayCallback.m_hitNormalWorld);
			raycast->m_HitObject = static_cast<GameObject*>(rayCallback.m_collisionObject->getUserPointer());
		}
		// ヒットしなかった場合の設定
		else 
		{
			raycast->m_IsHit = false;
			raycast->m_HitPosition = XMFLOAT3{};
			raycast->m_HitDistance = -1.0f;
			raycast->m_HitNormal = XMFLOAT3{};
			raycast->m_HitObject = nullptr;
		}
	}
}



std::vector<GameObject*> PhysicsSystem::GetCollisionEnter(GameObject* obj)
{
	// 衝突開始オブジェクトリスト
	std::vector<GameObject*> enterObjects;

	for (auto* currentObj : m_CurrentCollisions[obj])
	{
		// 前回の衝突情報に存在しない場合、衝突開始
		auto it = std::find(m_PreviousCollisions[obj].begin(), m_PreviousCollisions[obj].end(), currentObj);

		if (it == m_PreviousCollisions[obj].end())
			enterObjects.push_back(currentObj);
	}

	return enterObjects;
}

std::vector<GameObject*> PhysicsSystem::GetCollisionStay(GameObject* obj)
{
	// 衝突継続オブジェクトリスト
	std::vector<GameObject*> stayObjects;

	for (auto* currentObj : m_CurrentCollisions[obj])
	{
		// 前回の衝突情報に存在する場合、衝突継続
		auto it = std::find(m_PreviousCollisions[obj].begin(), m_PreviousCollisions[obj].end(), currentObj);

		if (it != m_PreviousCollisions[obj].end())
			stayObjects.push_back(currentObj);
	}

	return stayObjects;
}

std::vector<GameObject*> PhysicsSystem::GetCollisionExit(GameObject* obj)
{
	// 衝突終了オブジェクトリスト
	std::vector<GameObject*> exitObjects;

	for (auto* previousObj : m_PreviousCollisions[obj])
	{
		// 現在の衝突情報に存在しない場合、衝突終了
		auto it = std::find(m_CurrentCollisions[obj].begin(), m_CurrentCollisions[obj].end(), previousObj);

		if (it == m_CurrentCollisions[obj].end())
			exitObjects.push_back(previousObj);
	}

	return exitObjects;
}

std::vector<GameObject*> PhysicsSystem::GetTriggerEnter(GameObject* obj)
{
	// トリガー開始オブジェクトリスト
	std::vector<GameObject*> enterObjects;

	for (auto* currentObj : m_CurrentTriggers[obj])
	{
		// 前回のトリガー情報に存在しない場合、トリガー開始
		auto it = std::find(m_PreviousTriggers[obj].begin(), m_PreviousTriggers[obj].end(), currentObj);

		if (it == m_PreviousTriggers[obj].end())
			enterObjects.push_back(currentObj);
	}

	return enterObjects;
}

std::vector<GameObject*> PhysicsSystem::GetTriggerStay(GameObject* obj)
{
	// トリガー継続オブジェクトリスト
	std::vector<GameObject*> stayObjects;

	for (auto* currentObj : m_CurrentTriggers[obj])
	{
		// 前回のトリガー情報に存在する場合、トリガー継続
		auto it = std::find(m_PreviousTriggers[obj].begin(), m_PreviousTriggers[obj].end(), currentObj);

		if (it != m_PreviousTriggers[obj].end())
			stayObjects.push_back(currentObj);
	}

	return stayObjects;
}

std::vector<GameObject*> PhysicsSystem::GetTriggerExit(GameObject* obj)
{
	// トリガー終了オブジェクトリスト
	std::vector<GameObject*> exitObjects;

	for (auto* previousObj : m_PreviousTriggers[obj])
	{
		// 現在のトリガー情報に存在しない場合、トリガー終了
		auto it = std::find(m_CurrentTriggers[obj].begin(), m_CurrentTriggers[obj].end(), previousObj);

		if (it == m_CurrentTriggers[obj].end())
			exitObjects.push_back(previousObj);
	}

	return exitObjects;
}