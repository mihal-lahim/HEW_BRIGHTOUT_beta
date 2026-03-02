/*==============================================================================

   ローディング動画シーン [LoadingMovieScene.cpp]
--------------------------------------------------------------------------------

==============================================================================*/
#include "LoadingMovieScene.h"
#include "VideoPlayerController.h"
#include "GameObject.h"
#include <Windows.h>

void LoadingMovieScene::Initialize()
{
	OutputDebugStringA("[LoadingMovieScene] Initialize()\n");

	// 動画プレイヤー用のGameObjectを作成
	GameObject* videoPlayerObj = CreateGameObject();

	// VideoPlayerControllerを追加して動画パスを設定
	auto* controller = videoPlayerObj->AddComponent<VideoPlayerController>();
	controller->videoPath = L"movie/loading.mp4";
	controller->transitionDelay = 3.0f;
}

void LoadingMovieScene::Finalize()
{
	OutputDebugStringA("[LoadingMovieScene] Finalize()\n");
}
