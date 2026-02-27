#include "Result.h"
#include "GameObject.h"
#include "ResultController.h"
#include "RenderingSystem.h"
#include "audio.h"
#include "UIDrawer.h"
#include "ResultScoreUI.h"
#include "FadeController.h"

#include <optional>

static int g_ResultBgm{};
static std::optional<std::string> g_resultBgmPath = std::nullopt;

void SetResultBgm(const std::string& path)
{
	g_resultBgmPath = path;
}

void Result::Initialize()
{
	g_ResultBgm = LoadAudio("sound/Result_01.wav");
	// フェードイン前に画面を黒くしておく（灰色が一瞬映るのを防ぐ）
	rendering().GetGraphicsDevice().SetClearColor(0.0f,0.0f,0.0f,1.0f);

	// オーディオ初期化
	InitAudio();

	// 使用する BGM を決定（デフォルトまたは外部で指定されたパス）
	const char* bgmPath = "sound/Result_01.wav";
	if (g_resultBgmPath.has_value())
	{
		bgmPath = g_resultBgmPath->c_str();
	}

	g_ResultBgm = LoadAudio(bgmPath);

	PlayAudio(g_ResultBgm, true);
	SetAudioVolume(g_ResultBgm,0.2f);

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
		Vector3(0.0f,0.0f,0.0f),
		Vector3(1920.0f,1080.0f,1.0f),
		"UiVS.cso",
		"UiPS.cso"
	);

	// スコア表示UI
	GameObject* scoreObj = CreateGameObject();
	scoreObj->SetName("ResultScoreUI");
	scoreObj->AddComponent<ResultScoreUI>();

	// フェードイン演出
	GameObject* fadeObj = CreateGameObject();
	fadeObj->SetName("FadeController");
	auto* fadeCtrl = fadeObj->AddComponent<FadeController>();
	fadeCtrl->StartFadeIn(1.0f);

	// Reset the optional path after using it
	g_resultBgmPath = std::nullopt;
}

void Result::Finalize()
{
	// 音声解放
	StopAudio(g_ResultBgm);
	UnloadAudio(g_ResultBgm);
}
