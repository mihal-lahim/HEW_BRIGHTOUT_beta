#include "Title.h"
#include "GameObject.h"
#include "TitleController.h"
#include "RenderingSystem.h"
#include "audio.h"
#include "UIDrawer.h"

static int g_TitleBgm{};

void Title::Initialize()
{
	 //オーディオ初期化
	InitAudio();

	g_TitleBgm = LoadAudio("sound/GameTitle_02.wav");

	// デフォルトの背景色に戻す
	rendering().GetGraphicsDevice().SetClearColor(0.5f, 0.5f, 0.5f);

	// タイトルシーンの制御用GameObjectを作成
	GameObject* controller = CreateGameObject();
	controller->AddComponent<TitleController>();

	// タイトルUI表示
	GameObject* uiRoot = CreateGameObject();
	uiRoot->SetName("TitleUIRoot");

	UI::CreateUI(
		uiRoot,
		L"texture/Title.png",
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(1920.0f, 1080.0f, 1.0f),
		"UiVS.cso",
		"UiPS.cso"
	);

	 //BGM再生（ループ）
	PlayAudio(g_TitleBgm, true);

	SetAudioVolume(g_TitleBgm, 0.5f);
}

void Title::Finalize()
{
	// 音声解放
	UnloadAudio(g_TitleBgm);

	// XAudio終了
	UninitAudio();
}
