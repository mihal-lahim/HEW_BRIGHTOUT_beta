#include "Game.h"
#include "DynamicCubePrefab.h"
#include "StaticCubePrefab.h"
#include "CameraPrefab.h"

using namespace DirectX;

void Game::Initialize()
{
	// カメラプレハブをインスタンス化
	CameraPrefab cameraPrefab{};
	GameObject* cameraObject = Instantiate(cameraPrefab);
	cameraObject->transform().position() = { 0.0f, 2.0f, -5.0f };

	// キュープレハブをインスタンス化
	DynamicCubePrefab cubePrefab{};
	for (int i = -5; i < 5; ++i)
	{
		GameObject* cubeObject = Instantiate(cubePrefab);
		cubeObject->transform().position() = { static_cast<float>(i * 2), 5.0f, 0.0f };
	}

	StaticCubePrefab staticCubePrefab{};
	GameObject* groundObject = Instantiate(staticCubePrefab);
	groundObject->transform().position() = { 0.0f, -1.0f, 0.0f };
	groundObject->transform().scale() = { 20.0f, 1.0f, 20.0f };
}
