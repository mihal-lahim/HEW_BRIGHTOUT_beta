#include "Game.h"
#include "CollisionPrefab.h"
#include "PlayerPrefab.h"

using namespace DirectX;

void Game::Initialize()
{
	PlayerPrefab playerPrefab{};
	GameObject* player = Instantiate(playerPrefab);
	player->transform().position() = { 0.0f, 1.0f, 0.0f };

	StaticCubePrefab groundPrefab{};
	GameObject* groundObject = Instantiate(groundPrefab);
	groundObject->transform().scale() = { 50.0f, 1.0f, 50.0f };
}
