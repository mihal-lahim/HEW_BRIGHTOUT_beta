#include "Game.h"
#include "PlayerPrefab.h"
#include "GameObject.h"
#include "DebugCamera.h"
#include "Texture.h"

using namespace DirectX;

void Game::Initialize()
{
	ModelPrefab cubePrefab{ "model/cube.glb" };
	GameObject* floor = Instantiate(cubePrefab);
	floor->transform().scale() = { 50.0f, 1.0f, 50.0f };
	floor->transform().position() = { 0.0f, -1.0f, 0.0f };
	BoxColliderDesc floorShapeDesc{};
	floor->AddComponent<ColliderShape>(floorShapeDesc);
	PhysicsBodyDesc floorBodyDesc{};
	floorBodyDesc.Type = BodyType::STATIC;
	floor->AddComponent<PhysicsBody>(floorBodyDesc);

	ModelPrefab clockTowerPrefab{ "model/intersection.glb" };
	Instantiate(clockTowerPrefab);

	PlayerPrefab playerPrefab{};
	Instantiate(playerPrefab)->transform().position() += Vector3(5.0f, 0.0f, 0.0f);
	
}
