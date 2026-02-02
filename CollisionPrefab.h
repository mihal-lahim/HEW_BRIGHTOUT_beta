#ifndef COLLISION_PREFAB_H
#define COLLISION_PREFAB_H

#include "Prefab.h"
#include "PrimitivePrefab.h"
#include "GameObject.h"
#include "ColliderShape.h"
#include "PhysicsBody.h"

class StaticCubePrefab : public CubePrefab
{
	// インスタンス化メソッド
	void Instantiate(GameObject& gameObject) override
	{
		BoxColliderDesc boxDesc{};
		boxDesc.Friction = 0.5f;
		boxDesc.Restitution = 0.5f;
		gameObject.AddComponent<ColliderShape>(boxDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		gameObject.AddComponent<PhysicsBody>(bodyDesc);

		CubePrefab::Instantiate(gameObject);
	}
};

class StaticSpherePrefab : public SpherePrefab
{
	void Instantiate(GameObject& gameObject) override
	{
		SphereColliderDesc sphereDesc{};
		gameObject.AddComponent<ColliderShape>(sphereDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		gameObject.AddComponent<PhysicsBody>(bodyDesc);

		SpherePrefab::Instantiate(gameObject);
	}
};

class DynamicSpherePrefab : public SpherePrefab
{
	void Instantiate(GameObject& gameObject) override
	{
		SphereColliderDesc sphereDesc{};
		gameObject.AddComponent<ColliderShape>(sphereDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::DYNAMIC;
		gameObject.AddComponent<PhysicsBody>(bodyDesc);

		SpherePrefab::Instantiate(gameObject);
	}
};

class StaticCapsulePrefab : public CapsulePrefab
{
	void Instantiate(GameObject& gameObject) override
	{
		CapsuleColliderDesc capsuleDesc{};
		gameObject.AddComponent<ColliderShape>(capsuleDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		gameObject.AddComponent<PhysicsBody>(bodyDesc);

		CapsulePrefab::Instantiate(gameObject);
	}
};

class DynamicCapsulePrefab : public CapsulePrefab
{
	void Instantiate(GameObject& gameObject) override
	{
		CapsuleColliderDesc capsuleDesc{};
		gameObject.AddComponent<ColliderShape>(capsuleDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::DYNAMIC;
		gameObject.AddComponent<PhysicsBody>(bodyDesc);
		
		CapsulePrefab::Instantiate(gameObject);
	}
};

class StaticCylinderPrefab : public CylinderPrefab
{
	void Instantiate(GameObject& gameObject) override
	{
		CylinderColliderDesc cylinderDesc{};
		gameObject.AddComponent<ColliderShape>(cylinderDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		gameObject.AddComponent<PhysicsBody>(bodyDesc);

		CylinderPrefab::Instantiate(gameObject);
	}
};

class DynamicCylinderPrefab : public CylinderPrefab
{
	void Instantiate(GameObject& gameObject) override
	{
		CylinderColliderDesc cylinderDesc{};
		gameObject.AddComponent<ColliderShape>(cylinderDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::DYNAMIC;
		gameObject.AddComponent<PhysicsBody>(bodyDesc);

		CylinderPrefab::Instantiate(gameObject);
	}
};

class DynamicCubePrefab : public CubePrefab
{
	void Instantiate(GameObject& gameObject) override
	{
		BoxColliderDesc boxDesc{};
		gameObject.AddComponent<ColliderShape>(boxDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::DYNAMIC;
		gameObject.AddComponent<PhysicsBody>(bodyDesc);

		CubePrefab::Instantiate(gameObject);
	}
};



#endif
