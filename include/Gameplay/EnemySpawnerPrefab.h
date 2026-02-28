#ifndef ENEMY_SPAWNER_PREFAB_H
#define ENEMY_SPAWNER_PREFAB_H

#include "Prefab.h"
#include "EnemySpawner.h"
#include "GameObject.h"

class EnemySpawnerPrefab : public Prefab
{
public:
	// ‰Šú¶¬ŠÔŠui•bj
	float InitialInterval = 5.0f;
	// Å¬¶¬ŠÔŠui•bj
	float MinInterval = 1.0f;
	// ŠÔŠuŒ¸­‘¬“xi•b/•bj
	float IntervalDecreaseRate = 0.1f;
	// Å‘å“G”
	int MaxEnemies = 10;
	// U•z”¼Œa
	float SpawnRadius = 3.0f;

	EnemySpawnerPrefab() = default;
	virtual ~EnemySpawnerPrefab() = default;

	void Instantiate(GameObject& gameObject) override
	{
		auto* spawner = gameObject.AddComponent<EnemySpawner>();
		spawner->InitialInterval = InitialInterval;
		spawner->MinInterval = MinInterval;
		spawner->IntervalDecreaseRate = IntervalDecreaseRate;
		spawner->MaxEnemies = MaxEnemies;
		spawner->SpawnRadius = SpawnRadius;

		gameObject.SetTag("EnemySpawner");
	}
};

#endif
