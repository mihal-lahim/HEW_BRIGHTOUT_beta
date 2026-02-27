#include "Result.h"
#include "GameObject.h"
#include "ResultController.h"
#include "RenderingSystem.h"
#include "audio.h"
#include "UIDrawer.h"
#include "ResultScoreUI.h"

static int g_ResultBgm{};

void Result::Initialize()
{
	g_ResultBgm = LoadAudio("sound/Result_01.wav");

	PlayAudio(g_ResultBgm, true);
	SetAudioVolume(g_ResultBgm, 0.2f);

	// リザルトシーンの背景色を変更（青系の色で区別しやすくする）
	rendering().GetGraphicsDevice().SetClearColor(0.1f, 0.2f, 0.5f);

	// リザルトシーンの制御用GameObjectを作成
	GameObject* controller = CreateGameObject();
	controller->AddComponent<ResultController>();

	// リザルトUI表示
	GameObject* uiRoot = CreateGameObject();
	uiRoot->SetName("ResultUIRoot");

	UI::CreateUI(
		uiRoot,
		L"texture/BackGround.png",
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(1920.0f, 1080.0f, 1.0f),
		"UiVS.cso",
		"UiPS.cso"
	);

	UI::CreateUI(
		uiRoot,
		L"texture/Result.png",
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(1920.0f, 1080.0f, 1.0f),
		"UiVS.cso",
		"UiPS.cso"
	);

	// スコア表示UI
	GameObject* scoreObj = CreateGameObject();
	scoreObj->SetName("ResultScoreUI");
	scoreObj->AddComponent<ResultScoreUI>();
}

void Result::Finalize()
{
	// 音声解放
	StopAudio(g_ResultBgm);
	UnloadAudio(g_ResultBgm);
}
