#ifndef ENEMY_PREFAB_H
#define ENEMY_PREFAB_H

#include "Prefab.h"
#include "Enemy.h"
#include "Model.h"
#include "GameObject.h"
#include "Quaternion.h"

class EnemyPrefab : public Prefab
{
public:
	EnemyPrefab() = default;
	virtual ~EnemyPrefab() = default;

	void Instantiate(GameObject& gameObject) override
	{
		gameObject.AddComponent<Enemy>();
		gameObject.SetTag("Enemy");

		ModelPrefab modelPrefab{ "model/model_dake.fbx" };
		GameObject* modelObject = gameObject.Instantiate(modelPrefab);
		gameObject.SetChild(*modelObject);
		modelObject->transform().scale() = Vector3(0.01f, 0.01f, 0.01f);
		modelObject->transform().rotation() = Quaternion::SetEulerY(180.0f);
	}
};

#endif
