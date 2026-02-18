#include "Result.h"
#include "GameObject.h"
#include "ResultController.h"
#include "RenderingSystem.h"
#include "audio.h"	

static int g_ResultBgm{};

void Result::Initialize()
{
	//オーディオ初期化
	InitAudio();

	g_ResultBgm = LoadAudio("sound/GameResult_01.wav");

	// リザルトシーンの背景色を変更（青系の色で区別しやすくする）
	rendering().GetGraphicsDevice().SetClearColor(0.1f, 0.2f, 0.5f);

	// リザルトシーンの制御用GameObjectを作成
	GameObject* controller = CreateGameObject();
	controller->AddComponent<ResultController>();
}

void Result::Finalize()
{
	// 音声解放
	UnloadAudio(g_ResultBgm);

	// XAudio終了
	UninitAudio();
}
