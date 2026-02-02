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
		gameObject.AddComponent<ColliderShape>(boxDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::DYNAMIC;
		gameObject.AddComponent<PhysicsBody>(bodyDesc);

		CubePrefab::Instantiate(gameObject);
	}
};



#endif
