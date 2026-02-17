#include "GameClearChecker.h"
#include "GameObject.h"
#include "SceneSystem.h"
#include "PowerPlant.h"
#include "Result.h"
#include "Scene.h"

void GameClearChecker::Update()
{
	// シーン内の全PowerPlantコンポーネントを取得
	auto plants = scene().currentScene().GetComponents<PowerPlant>();

	if (plants.empty())
		return;

	// 全ての発電所が復旧済みかチェック
	for (auto* plant : plants)
	{
		if (!plant->IsRestored())
			return;
	}

	// 全て復旧済み → リザルトシーンへ遷移
	scene().ChangeScene<Result>();
}
