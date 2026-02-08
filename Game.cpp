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
	cube->transform().scale() = Vector3(1.0f, 1.0f, 1.0f);

	
	PlayerPrefab playerPrefab{};
	Instantiate(playerPrefab);
	
}
