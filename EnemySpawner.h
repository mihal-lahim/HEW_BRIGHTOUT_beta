#ifndef ENEMY_SPAWNER_H
#define ENEMY_SPAWNER_H

#include "ScriptComponent.h"
#include "EnemyPrefab.h"
#include "Time.h"
#include "GameObject.h"
#include <algorithm>

// 敵スポナークラス
// 各区画に設置し、一定間隔で敵を生成する
// 時間経過とともに生成間隔が短くなる
class EnemySpawner : public ScriptComponent
{
public:
	// 初期生成間隔（秒）
	float InitialInterval = 5.0f;
	// 最小生成間隔（秒）
	float MinInterval = 1.0f;
	// 生成間隔の減少速度（秒/秒）
	float IntervalDecreaseRate = 0.1f;
	// 同時に存在できる敵の最大数
	int MaxEnemies = 20;
	// 生成位置のランダム散布半径
	float SpawnRadius = 3.0f;

	EnemySpawner() = default;
	virtual ~EnemySpawner() = default;

	void Awake() override
	{
		m_CurrentInterval = InitialInterval;
		m_Timer = 0.0f;
		m_ElapsedTime = 0.0f;
		m_SpawnCount = 0;

		// 初回敵生成（敵コンポーネントプールの事前登録を兼ねる）
		SpawnEnemy();
	}

	void Update() override
	{
		float dt = (float)Time::DeltaTime();
		m_ElapsedTime += dt;

		// 時間経過で生成間隔を短くする
		m_CurrentInterval = (std::max)(InitialInterval - IntervalDecreaseRate * m_ElapsedTime, MinInterval);

		m_Timer += dt;
		m_SpawnCount = static_cast<int>(GetGameObjectsByTag("Enemy").size());
		while (m_Timer >= m_CurrentInterval && m_SpawnCount < MaxEnemies)
		{
			m_Timer -= m_CurrentInterval;

			// 最大数に達していなければ生成
			SpawnEnemy();
			++m_SpawnCount;
		}
	}

	// 現在の生成間隔を取得
	float GetCurrentInterval() const { return m_CurrentInterval; }

	// 生成済み敵数を取得
	int GetSpawnCount() const { return m_SpawnCount; }

	// 敵が倒された時に呼び出す（外部から）
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

		// スポナーの位置を基準にランダムオフセットで配置
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
