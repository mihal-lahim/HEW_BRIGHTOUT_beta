
#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Ray.h"
#include "GameObject.h"



using namespace DirectX;

btTransform PhysicsSystem::ApplyOffsets(Collider& collider)
{
	// ���L�҂̃Q�[���I�u�W�F�N�g��Transform�擾
	Transform* tf = &collider.gameObject()->transform;

	// �ʒu�ݒ�
	// Static colliders use world space, rigidbody colliders use local offsets in compound shapes.
	XMVECTOR pos = XMLoadFloat3(&collider.m_OffsetPos);
	if (collider.m_IsStatic)
	{
		pos = XMVectorAdd(pos, XMLoadFloat3(&tf->Position));
	}
	XMFLOAT3 btPos{};
	XMStoreFloat3(&btPos, pos);

	// ��]�ݒ�
	Quaternion ownerRot = tf->Rotation;
	Quaternion offsetRot = collider.m_OffsetRot;

	// Static colliders combine owner rotation; rigidbody colliders use local rotation only.
	XMFLOAT4 combinedRot{};
	if (collider.m_IsStatic)
	{
		combinedRot = (ownerRot * offsetRot).Quat;
	}
	else
	{
		combinedRot = offsetRot.Quat;
	}

	// �g�����X�t�H�[���ݒ�
	btTransform bttf{};
	bttf.setOrigin(ToBulletPosition(btPos));
	bttf.setRotation(ToBulletRotation(Quaternion{ combinedRot }));


	// �T�C�Y�ݒ�
	XMFLOAT3 scale = collider.m_Scale;
	XMFLOAT3 ownerScale = tf->Scale;

	scale.x *= ownerScale.x;
	scale.y *= ownerScale.y;
	scale.z *= ownerScale.z;

	btVector3 size = btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f);

	// �R���W�����`��ݒ�
	switch (collider.m_Type)
	{
	case ColliderType::BOX:
		collider.m_Shape = std::make_unique<btBoxShape>(size);
		break;
	case ColliderType::SPHERE:
		collider.m_Shape = std::make_unique<btSphereShape>(size.x());
		break;
	case ColliderType::CAPSULE:
		collider.m_Shape = std::make_unique<btCapsuleShape>(size.x(), size.y());
		break;
	case ColliderType::CYLINDER:
		collider.m_Shape = std::make_unique<btCylinderShape>(size);
		break;
	}

	return bttf;
}

void PhysicsSystem::Initialize()
{
	m_Broadphase = std::make_unique<btDbvtBroadphase>();
	m_CollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	m_Dispatcher = std::make_unique<btCollisionDispatcher>(m_CollisionConfiguration.get());
	m_Solver = std::make_unique<btSequentialImpulseConstraintSolver>();

	m_DynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
		m_Dispatcher.get(),
		m_Broadphase.get(),
		m_Solver.get(),
		m_CollisionConfiguration.get());

	m_DynamicsWorld->setGravity(btVector3(0, 0.0f, 0));
}

void PhysicsSystem::Finalize()
{
	m_DynamicsWorld.reset();
	m_Solver.reset();
	m_Dispatcher.reset();
	m_CollisionConfiguration.reset();
	m_Broadphase.reset();
}

void PhysicsSystem::RegisterCollider(Collider* collider)
{
	// ��������PhysicsSystem�ݒ�
	collider->m_PhysicsSystem = this;

	// �I�t�Z�b�g�K�p
	btTransform bttf = ApplyOffsets(*collider);

	// �ÓI�R���C�_�[�̏ꍇ�X�L�b�v
	if (!collider->m_IsStatic) return;

	// �R���W�����I�u�W�F�N�g�쐬
	btCollisionObject* obj = new btCollisionObject();

	// �R���W�����`��ݒ�
	obj->setCollisionShape(collider->m_Shape.get());

	// �g�����X�t�H�[���ݒ�
	obj->setWorldTransform(bttf);

	// ���[�U�[�|�C���^�ݒ�
	obj->setUserPointer(collider->gameObject());

	// �g���K�[�ݒ�
	if (collider->m_IsTrigger)
		obj->setCollisionFlags(obj->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	// �R���C�_�[�o�^
	m_DynamicsWorld->addCollisionObject(obj);

	collider->m_CollisionObject = std::unique_ptr<btCollisionObject>(obj);
}

void PhysicsSystem::RegisterRigidBody(RigidBody* rigidbody)
{
	// ��������PhysicsSystem�ݒ�
	rigidbody->m_PhysicsSystem = this;

	// �R���p�E���h�V�F�C�v�쐬
	rigidbody->m_CompoundShape = std::make_unique<btCompoundShape>();
	btCompoundShape* compoundShape = rigidbody->m_CompoundShape.get();

	for (auto* collider : rigidbody->m_Colliders)
	{
		// �I�t�Z�b�g�K�p
		btTransform bttf = ApplyOffsets(*collider);

		// �R���p�E���h�V�F�C�v�ɒǉ�
		compoundShape->addChildShape(bttf, collider->m_Shape.get());
	}


	// �����e���\��
	btVector3 localInertia(0, 0, 0);

	// ���ʂ�����ꍇ�͊����e���\�����v�Z
	if (rigidbody->m_Mass > 0.0f)
		compoundShape->calculateLocalInertia(rigidbody->m_Mass, localInertia);

	// �Œ肳����]���̊����e���\����0�ɐݒ�
	localInertia.setX(rigidbody->m_FixedRotation.x == 1.0f ? 0.0f : localInertia.x());
	localInertia.setY(rigidbody->m_FixedRotation.y == 1.0f ? 0.0f : localInertia.y());
	localInertia.setZ(rigidbody->m_FixedRotation.z == 1.0f ? 0.0f : localInertia.z());


	// �����ʒu�ݒ�
	btTransform startPos;
	startPos.setOrigin(ToBulletPosition(rigidbody->gameObject()->transform.Position));
	startPos.setRotation(ToBulletRotation(rigidbody->gameObject()->transform.Rotation));

	// ���[�V�����X�e�[�g�쐬
	btDefaultMotionState* motionState = new btDefaultMotionState(startPos);

	// ���̍쐬���ݒ�
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		rigidbody->m_Mass,
		motionState,
		compoundShape,
		localInertia);

	// ���̍쐬
	btRigidBody* body = new btRigidBody(rbInfo);


	// ���̓o�^
	m_DynamicsWorld->addRigidBody(body);


	// �g���K�[�ݒ�
	if (rigidbody->m_IsTrigger)
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	// ���[�U�[�|�C���^�ݒ�
	body->setUserPointer(rigidbody->gameObject());



	// XMFLOAT3��btVector3�ɕϊ�
	btVector3 gravity = ToBulletPosition(rigidbody->m_Gravity);

	// �d�͂̐ݒ�
	body->setGravity(gravity);



	// XMFLOAT3��btVector3�ɕϊ�
	btVector3 angularFactor = ToBulletPosition(rigidbody->m_FixedRotation);

	// �p�x�̌Œ�ݒ�
	body->setAngularFactor(angularFactor);


	// �����o�ϐ��ɐݒ�
	rigidbody->m_RigidBody = std::unique_ptr<btRigidBody>(body);
	rigidbody->m_MotionState = std::unique_ptr<btMotionState>(motionState);

	// �o�^����Ă��鍄�̃��X�g�ɒǉ�
	m_RigidBodies.push_back(rigidbody);
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
	// �������Z�X�e�b�v
	m_DynamicsWorld->stepSimulation(deltaTime);
}

#include "debug_ostream.h"

void PhysicsSystem::UpdateRigidBody()
{
	for (auto* rigidbody : m_RigidBodies)
	{
		// �g�����X�t�H�[���擾
		btTransform worldTransform = rigidbody->m_RigidBody->getWorldTransform();

		// �ʒu�X�V
		Transform* tf = &rigidbody->gameObject()->transform;

		tf->Position = ToDirectXPosition(worldTransform.getOrigin());
		tf->Rotation = ToDirectXRotation(worldTransform.getRotation());

		hal::dout << "Position: " << tf->Position.x << ", " << tf->Position.y << ", " << tf->Position.z << std::endl;

		// �͂����Z�b�g
		rigidbody->m_RigidBody->clearForces();
	}
}

void PhysicsSystem::UpdateCollisions()
{
	// �O��̏Փˏ���ۑ�
	m_PreviousCollisions = m_CurrentCollisions;

	// �Փˏ��̎擾
	int numManifolds = m_DynamicsWorld->getDispatcher()->getNumManifolds();

	for (int i = 0; i < numManifolds; i++)
	{
		// �Փ˃}�j�z�[���h�擾
		btPersistentManifold* contactManifold = m_DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		// �Փ˃I�u�W�F�N�g�擾
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		// �Q�[���I�u�W�F�N�g�擾
		GameObject* gameObjectA = static_cast<GameObject*>(obA->getUserPointer());
		GameObject* gameObjectB = static_cast<GameObject*>(obB->getUserPointer());

		// �Փ˓_���擾
		int numContacts = contactManifold->getNumContacts();

		for (int j = 0; j < numContacts; j++)
		{
			// �Փ˓_�擾
			btManifoldPoint& pt = contactManifold->getContactPoint(j);

			// �Փ˂��Ă��邩�m�F
			if (pt.getDistance() < 0.0f)
			{
				// �g���K�[���ǂ����m�F
				bool isTriggerA = obA->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE;
				bool isTriggerB = obB->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE;

				// �g���K�[�̏ꍇ
				if (isTriggerA || isTriggerB)
				{
					// �g���K�[���ɒǉ�
					m_CurrentTriggers[gameObjectA].push_back(gameObjectB);
					m_CurrentTriggers[gameObjectB].push_back(gameObjectA);
				}
				// �g���K�[�łȂ��ꍇ
				else
				{
					// �Փˏ��ɒǉ�
					m_CurrentCollisions[gameObjectA].push_back(gameObjectB);
					m_CurrentCollisions[gameObjectB].push_back(gameObjectA);
				}
			}
		}
	}
}

void PhysicsSystem::RayCast(Ray& ray, float distance)
{
	// ���C�̕����𐳋K�����ċ������|����
	XMVECTOR dir = XMLoadFloat3(&ray.m_Direction);
	dir = XMVector3Normalize(dir);
	dir = XMVectorScale(dir, distance);

	// ���C�̏I�_�v�Z
	XMFLOAT3 toFloat3{};
	XMStoreFloat3(&toFloat3, XMVectorAdd(dir, XMLoadFloat3(&ray.m_From)));

	// �n�_�ƏI�_�����x�N�g�����[���x�N�g���̏ꍇ�͏������X�L�b�v
	if (!XMVectorGetX(XMVectorEqual(XMVectorSubtract(XMLoadFloat3(&toFloat3), XMLoadFloat3(&ray.m_From)), XMVectorZero())) == 0.0f)
	{
		// ���C�̎n�_�ƏI�_��Bullet�̌`���ɕϊ�
		btVector3 from = ToBulletPosition(ray.m_From);
		btVector3 to = ToBulletPosition(toFloat3);

		// ���C�L���X�g�̎��s
		btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);

		// ���C�e�X�g���s
		m_DynamicsWorld->rayTest(from, to, rayCallback);

		// ���C���q�b�g���ATrigger�𖳎�����ݒ�
		if (rayCallback.hasHit() && !(rayCallback.m_collisionObject->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE))
		{
			// �q�b�g����RayCast�ɐݒ�
			ray.IsHit = true;
			ray.HitPosition = ToDirectXPosition(rayCallback.m_hitPointWorld);
			ray.HitDistance = (rayCallback.m_hitPointWorld - from).length();
			ray.HitNormal = ToDirectXPosition(rayCallback.m_hitNormalWorld);
			ray.HitObject = static_cast<GameObject*>(rayCallback.m_collisionObject->getUserPointer());
			return;
		}
	}

	// �q�b�g���Ȃ������ꍇ�̏�����
	ray.IsHit = false;
	ray.HitPosition = XMFLOAT3{};
	ray.HitDistance = -1.0f;
	ray.HitNormal = XMFLOAT3{};
	ray.HitObject = nullptr;
}



std::vector<GameObject*> PhysicsSystem::GetCollisionEnter(GameObject* obj)
{
	// �ՓˊJ�n�I�u�W�F�N�g���X�g
	std::vector<GameObject*> enterObjects;

	for (auto* currentObj : m_CurrentCollisions[obj])
	{
		// �O��̏Փˏ��ɑ��݂��Ȃ��ꍇ�A�ՓˊJ�n
		auto it = std::find(m_PreviousCollisions[obj].begin(), m_PreviousCollisions[obj].end(), currentObj);

		if (it == m_PreviousCollisions[obj].end())
			enterObjects.push_back(currentObj);
	}

	return enterObjects;
}

std::vector<GameObject*> PhysicsSystem::GetCollisionStay(GameObject* obj)
{
	// �Փˌp���I�u�W�F�N�g���X�g
	std::vector<GameObject*> stayObjects;

	for (auto* currentObj : m_CurrentCollisions[obj])
	{
		// �O��̏Փˏ��ɑ��݂���ꍇ�A�Փˌp��
		auto it = std::find(m_PreviousCollisions[obj].begin(), m_PreviousCollisions[obj].end(), currentObj);

		if (it != m_PreviousCollisions[obj].end())
			stayObjects.push_back(currentObj);
	}

	return stayObjects;
}

std::vector<GameObject*> PhysicsSystem::GetCollisionExit(GameObject* obj)
{
	// �ՓˏI���I�u�W�F�N�g���X�g
	std::vector<GameObject*> exitObjects;

	for (auto* previousObj : m_PreviousCollisions[obj])
	{
		// ���݂̏Փˏ��ɑ��݂��Ȃ��ꍇ�A�ՓˏI��
		auto it = std::find(m_CurrentCollisions[obj].begin(), m_CurrentCollisions[obj].end(), previousObj);

		if (it == m_CurrentCollisions[obj].end())
			exitObjects.push_back(previousObj);
	}

	return exitObjects;
}

std::vector<GameObject*> PhysicsSystem::GetTriggerEnter(GameObject* obj)
{
	// �g���K�[�J�n�I�u�W�F�N�g���X�g
	std::vector<GameObject*> enterObjects;

	for (auto* currentObj : m_CurrentTriggers[obj])
	{
		// �O��̃g���K�[���ɑ��݂��Ȃ��ꍇ�A�g���K�[�J�n
		auto it = std::find(m_PreviousTriggers[obj].begin(), m_PreviousTriggers[obj].end(), currentObj);

		if (it == m_PreviousTriggers[obj].end())
			enterObjects.push_back(currentObj);
	}

	return enterObjects;
}

std::vector<GameObject*> PhysicsSystem::GetTriggerStay(GameObject* obj)
{
	// �g���K�[�p���I�u�W�F�N�g���X�g
	std::vector<GameObject*> stayObjects;

	for (auto* currentObj : m_CurrentTriggers[obj])
	{
		// �O��̃g���K�[���ɑ��݂���ꍇ�A�g���K�[�p��
		auto it = std::find(m_PreviousTriggers[obj].begin(), m_PreviousTriggers[obj].end(), currentObj);

		if (it != m_PreviousTriggers[obj].end())
			stayObjects.push_back(currentObj);
	}

	return stayObjects;
}

std::vector<GameObject*> PhysicsSystem::GetTriggerExit(GameObject* obj)
{
	// �g���K�[�I���I�u�W�F�N�g���X�g
	std::vector<GameObject*> exitObjects;

	for (auto* previousObj : m_PreviousTriggers[obj])
	{
		// ���݂̃g���K�[���ɑ��݂��Ȃ��ꍇ�A�g���K�[�I��
		auto it = std::find(m_CurrentTriggers[obj].begin(), m_CurrentTriggers[obj].end(), previousObj);

		if (it == m_CurrentTriggers[obj].end())
			exitObjects.push_back(previousObj);
	}

	return exitObjects;
}
