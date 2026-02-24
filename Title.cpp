#include "Title.h"
#include "GameObject.h"
#include "TitleController.h"
#include "RenderingSystem.h"
#include "audio.h"

static int g_TitleBgm{};

void Title::Initialize()
{
	 //オーディオ初期化
	InitAudio();

	g_TitleBgm = LoadAudio("sound/GameTitle_01.wav");

	// デフォルトの背景色に戻す
	rendering().GetGraphicsDevice().SetClearColor(0.5f, 0.5f, 0.5f);

	// タイトルシーンの制御用GameObjectを作成
	GameObject* controller = CreateGameObject();
	controller->AddComponent<TitleController>();

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
