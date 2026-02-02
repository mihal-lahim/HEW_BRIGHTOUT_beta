#include "Game.h"
#include "CollisionPrefab.h"
#include "PlayerPrefab.h"
#include "SceneSystem.h"
#include "Texture.h"
#include "Billboard.h"

using namespace DirectX;

void Game::Initialize()
{
	StaticCubePrefab groundPrefab{};
	groundPrefab.TexturePtr = resource().Load<Texture>(L"texture/ground.png");
	GameObject* groundObject = Instantiate(groundPrefab);
	groundObject->transform().scale() = { 100.0f, 1.0f, 100.0f };

	PlanePrefab plane{};
	plane.TexturePtr = resource().Load<Texture>(L"texture/crystal.png");
	GameObject* planeObject = Instantiate(plane);
	planeObject->transform().position() = { 0.0f, 20.0f, 0.0f };
	planeObject->transform().scale() = { 10.0f, 1.0f, 10.0f };
	planeObject->AddComponent<Billboard>();


	PlayerPrefab playerPrefab{};
	GameObject* player = Instantiate(playerPrefab);
	player->transform().position() = { 0.0f, 1.0f, 0.0f };
}
