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
		gameObject.AddComponent<ColliderShape>(boxDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		gameObject.AddComponent<PhysicsBody>(bodyDesc);

		CubePrefab::Instantiate(gameObject);
	}
};



#endif
