#include "Game.h"
#include "CollisionPrefab.h"
#include "PlayerPrefab.h"
#include "GameObject.h"

using namespace DirectX;

void Game::Initialize()
{
	PlayerPrefab playerPrefab{};
	GameObject* player = Instantiate(playerPrefab);
	player->transform().position() = Vector3(0.0f, 1.0f, 0.0f);

	StaticCubePrefab cubePrefab{};
	cubePrefab.TexturePtr = resource().Load<Texture>(L"texture/ground.png");
	GameObject* ground = Instantiate(cubePrefab);
	ground->transform().scale() = Vector3(500.0f, 1.0f, 500.0f);
}
