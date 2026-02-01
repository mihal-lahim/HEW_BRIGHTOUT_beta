#ifndef STATIC_CUBE_PREFAB_H
#define STATIC_CUBE_PREFAB_H

#include "Prefab.h"
#include "CubePrefab.h"
#include "GameObject.h"
#include "ColliderShape.h"
#include "PhysicsBody.h"

class StaticCubePrefab : public CubePrefab
{
	// インスタンス化メソッド
	void Instantiate(GameObject& gameObject) const override
	{
		BoxColliderDesc boxDesc{};
		BoxColliderShape shape(boxDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		PhysicsBody* physicsBody = gameObject.AddComponent<PhysicsBody>(bodyDesc);
		physicsBody->AddShape(shape);

		CubePrefab::Instantiate(gameObject);
	}
};



#endif
