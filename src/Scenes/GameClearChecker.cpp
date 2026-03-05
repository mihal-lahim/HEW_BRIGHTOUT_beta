#include "GameClearChecker.h"
#include "GameObject.h"
#include "SceneSystem.h"
#include "PowerPlant.h"
#include "Result.h"
#include "Scene.h"
#include "FadeController.h"

void GameClearChecker::Start()
{
	// フェードコントローラを自分のGameObjectに追加
	m_fadeController = gameObject().AddComponent<FadeController>();
}

void GameClearChecker::Update()
{
	// 遷移中は何もしない
	if (m_isTransitioning) return;

	// PowerPlantリストを初回のみキャッシュ
	if (!m_plantsCached)
	{
		m_cachedPlants = scene().currentScene().GetComponents<PowerPlant>();
		m_plantsCached = true;
	}

	if (m_cachedPlants.empty())
		return;

	// すべての発電所が復旧済みかチェック
	for (auto* plant : m_cachedPlants)
	{
		if (!plant || !plant->IsRestored())
			return;
	}

	// 全て復旧済み → フェードアウト開始してリザルトシーンへ遷移
	m_isTransitioning = true;

	// リザルトが発電所復旧完了の場合は Result_01.wav を再生するよう設定
	SetResultBgm("sound/Result_01.wav");

	m_fadeController->StartFadeOut(1.0f, [this]()
	{
		scene().ChangeScene<Result>();
	});
}
