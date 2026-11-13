#pragma once

#include "ScriptComponent.h"
#include "Vector3.h"
#include <vector>
#include <memory>
#include <string>

class Mesh;
class MeshRenderer;

// リザルト画面のスコア行を1つ表す設定
struct ScoreRowConfig
{
	Vector3 position;   // 行の左端位置（ピクセル座標）
	Vector3 digitScale; // 各桁のスケール
	float digitSpacing; // 桁間の間隔
	int maxDigits;      // 表示する最大桁数
};

class ResultScoreUI : public ScriptComponent
{
public:
	// 各項目の位置・スケール設定（
	ScoreRowConfig configRestoredCount  = { { 1200.0f, 300.0f, 0.0f }, { 60.0f, 60.0f, 1.0f }, 80.0f, 1 };
	ScoreRowConfig configRestoredScore  = { { 1000.0f, 370.0f, 0.0f }, { 60.0f, 60.0f, 1.0f }, 80.0f, 4 };
	ScoreRowConfig configKilledCount    = { { 1100.0f, 500.0f, 0.0f }, { 60.0f, 60.0f, 1.0f }, 90.0f, 3 };
	ScoreRowConfig configKilledScore    = { { 880.0f, 570.0f, 0.0f }, { 60.0f, 60.0f, 1.0f }, 90.0f, 5 };
	ScoreRowConfig configTotalScore     = { { 500.0f, 850.0f, 0.0f }, { 120.0f, 120.0f, 1.0f }, 120.0f, 6 };

	void Start() override;

private:
	// 0~9 の切り出しメッシュ
	std::vector<std::shared_ptr<Mesh>> m_digitMeshes;

	// 1行分の数字レンダラを作成する
	void CreateNumberRow(const ScoreRowConfig& config, int value);
};
