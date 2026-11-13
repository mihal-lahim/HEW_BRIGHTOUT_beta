#pragma once

// シーン間でスコアデータを保持するグローバル構造体
struct ScoreData
{
	// 直した発電所の数
	int restoredPlants = 0;

	// 倒した敵の数
	int killedEnemies = 0;

	// 発電所1つあたりのスコア
	static constexpr int PointsPerPlant = 1000;

	// 敵1体あたりのスコア
	static constexpr int PointsPerEnemy = 500;

	// 発電所スコア
	int GetPlantScore() const { return restoredPlants * PointsPerPlant; }

	// 敵スコア
	int GetEnemyScore() const { return killedEnemies * PointsPerEnemy; }

	// 合計スコア
	int GetTotalScore() const { return GetPlantScore() + GetEnemyScore(); }

	// リセット
	void Reset()
	{
		restoredPlants = 0;
		killedEnemies = 0;
	}

	// グローバルインスタンス
	static ScoreData& Instance()
	{
		static ScoreData s_instance;
		return s_instance;
	}
};
