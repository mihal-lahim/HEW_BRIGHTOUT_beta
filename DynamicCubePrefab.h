#ifndef DYNAMIC_CUBE_PREFAB_H
#define DYNAMIC_CUBE_PREFAB_H

#include "Prefab.h"
#include "CubePrefab.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ColliderShape.h"
#include "PhysicsBody.h"

class DynamicCubePrefab : public CubePrefab
{
	void Instantiate(GameObject& gameObject) const override
	{
		BoxColliderDesc boxDesc{};
		BoxColliderShape shape(boxDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::DYNAMIC;
		PhysicsBody* physicsBody = gameObject.AddComponent<PhysicsBody>(bodyDesc);
		physicsBody->AddShape(shape);

		CubePrefab::Instantiate(gameObject);
	}
};



#endif
