#ifndef ENEMY_SPAWNER_H
#define ENEMY_SPAWNER_H

#include "ScriptComponent.h"
#include "EnemyPrefab.h"
#include "GameTime.h"
#include "GameObject.h"
#include <algorithm>

// “GƒXƒ|ƒi[ƒNƒ‰ƒX
// Še‹æ‰æ‚ÉÝ’u‚µAˆê’èŠÔŠu‚Å“G‚ð¶¬‚·‚é
// ŽžŠÔŒo‰ß‚Æ‚Æ‚à‚É¶¬ŠÔŠu‚ª’Z‚­‚È‚é
class EnemySpawner : public ScriptComponent
{
public:
	// ‰Šú¶¬ŠÔŠui•bj
	float InitialInterval = 5.0f;
	// Å¬¶¬ŠÔŠui•bj
	float MinInterval = 1.0f;
	// ¶¬ŠÔŠu‚ÌŒ¸­‘¬“xi•b/•bj
	float IntervalDecreaseRate = 0.1f;
	// “¯Žž‚É‘¶Ý‚Å‚«‚é“G‚ÌÅ‘å”
	int MaxEnemies = 20;
	// ¶¬ˆÊ’u‚Ìƒ‰ƒ“ƒ_ƒ€ŽU•z”¼Œa
	float SpawnRadius = 3.0f;

	EnemySpawner() = default;
	virtual ~EnemySpawner() = default;

	void Awake() override
	{
		m_CurrentInterval = InitialInterval;
		m_Timer = 0.0f;
		m_ElapsedTime = 0.0f;
		m_SpawnCount = 0;

		// ‰‰ñ“G¶¬i“GƒRƒ“ƒ|[ƒlƒ“ƒgƒv[ƒ‹‚ÌŽ–‘O“o˜^‚ðŒ“‚Ë‚éj
		SpawnEnemy();
	}

	void Update() override
	{
		float dt = (float)Time::DeltaTime();
		m_ElapsedTime += dt;

		// ŽžŠÔŒo‰ß‚Å¶¬ŠÔŠu‚ð’Zk‚·‚é
		m_CurrentInterval = (std::max)(InitialInterval - IntervalDecreaseRate * m_ElapsedTime, MinInterval);

		m_Timer += dt;

		// ƒ^ƒOŒŸõ‚Í¶¬ƒ^ƒCƒ~ƒ“ƒO‚ÌŽž‚¾‚¯ŽÀs‚·‚é
		if (m_Timer >= m_CurrentInterval)
		{
			m_SpawnCount = static_cast<int>(GetGameObjectsByTag("Enemy").size());
			while (m_Timer >= m_CurrentInterval && m_SpawnCount < MaxEnemies)
			{
				m_Timer -= m_CurrentInterval;

				// Å‘å”‚É’B‚µ‚Ä‚¢‚È‚¯‚ê‚Î¶¬
				SpawnEnemy();
				++m_SpawnCount;
			}
		}
	}

	// Œ»Ý‚Ì¶¬ŠÔŠu‚ðŽæ“¾
	float GetCurrentInterval() const { return m_CurrentInterval; }

	// ¶¬Ï‚Ý“G”‚ðŽæ“¾
	int GetSpawnCount() const { return m_SpawnCount; }

	// “G‚ª“|‚³‚ê‚½Žž‚ÉŒÄ‚Ño‚·iŠO•”‚©‚çj
	void OnEnemyDestroyed() { if (m_SpawnCount > 0) --m_SpawnCount; }

private:
	float m_Timer = 0.0f;
	float m_ElapsedTime = 0.0f;
	float m_CurrentInterval = 5.0f;
	int m_SpawnCount = 0;

	void SpawnEnemy()
	{
		EnemyPrefab enemyPrefab;
		GameObject* enemy = Instantiate(enemyPrefab);
		if (!enemy) return;

		// ƒXƒ|ƒi[‚ÌˆÊ’u‚ðŠî€‚Éƒ‰ƒ“ƒ_ƒ€ƒIƒtƒZƒbƒg‚Å”z’u
		Vector3 spawnPos = gameObject().transform().position();
		float angle = (float)(rand() % 360) * 3.14159265f / 180.0f;
		float radius = (float)(rand() % 100) / 100.0f * SpawnRadius;
		spawnPos.x += cosf(angle) * radius;
		spawnPos.z += sinf(angle) * radius;
		enemy->transform().position() = spawnPos;

		++m_SpawnCount;
	}
};

#endif
