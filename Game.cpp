#include "Game.h"
#include "PlayerPrefab.h"
#include "GameObject.h"
#include "DebugCamera.h"
#include "Texture.h"

using namespace DirectX;

void Game::Initialize()
{
	/*
	CubePrefab cubePrefab{};
	GameObject* floor = Instantiate(cubePrefab);
	floor->transform().scale() = { 50.0f, 1.0f, 50.0f };
	floor->transform().position() = { 0.0f, -1.0f, 0.0f };
	BoxColliderDesc floorShapeDesc{};
	floor->AddComponent<ColliderShape>(floorShapeDesc);
	PhysicsBodyDesc floorBodyDesc{};
	floorBodyDesc.Type = BodyType::STATIC;
	floor->AddComponent<PhysicsBody>(floorBodyDesc);
	*/

	ModelPrefab modelPrefab{ "model/Player2.glb" };
	GameObject* sphere = Instantiate(modelPrefab);
	sphere->transform().scale() = { 10.0f, 10.0f, 10.0f };

	PlayerPrefab playerPrefab{};
	Instantiate(playerPrefab)->transform().position() += Vector3(5.0f, 0.0f, 0.0f);
	
}
