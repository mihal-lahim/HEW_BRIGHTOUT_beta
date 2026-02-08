#include "Game.h"
#include "PlayerPrefab.h"
#include "GameObject.h"
#include "DebugCamera.h"
#include "Texture.h"

using namespace DirectX;

void Game::Initialize()
{
	CubePrefab cubePrefab{};
	GameObject* cube = Instantiate(cubePrefab);
	cube->transform().scale() = Vector3(0.5f, 0.5f, 0.5f);

	GameObject* camera = CreateGameObject();
	camera->AddComponent<Camera>();
	camera->transform().position() = Vector3(0.0f, 0.0f, -5.0f);
}
