#include "Game.h"
#include "PlayerPrefab.h"
#include "GameObject.h"
#include "include/DebugTest/DebugCamera.h"
#include "Texture.h"
#include "PowerPlant.h"
#include "EnemySpawnerPrefab.h"
#include "Convenience.h"

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

	// 各区画にコンビニエンスストアを配置
	EnemySpawnerPrefab spawnerPrefab{};
	spawnerPrefab.InitialInterval = 5.0f;
	spawnerPrefab.MinInterval = 1.0f;
	spawnerPrefab.IntervalDecreaseRate = 0.1f;
	spawnerPrefab.MaxEnemies = 10;
	spawnerPrefab.SpawnRadius = 3.0f;

	// 区画1
	auto* store1 = Instantiate(spawnerPrefab);
	store1->AddComponent<Convenience>();
	store1->SetTag("Convenience");
	store1->transform().position() = Vector3(-20.0f, 0.0f, 20.0f);
	// 区画2
	auto* store2 = Instantiate(spawnerPrefab);
	store2->AddComponent<Convenience>();
	store2->SetTag("Convenience");
	store2->transform().position() = Vector3(20.0f, 0.0f, 20.0f);
	// 区画3
	auto* store3 = Instantiate(spawnerPrefab);
	store3->AddComponent<Convenience>();
	store3->SetTag("Convenience");
	store3->transform().position() = Vector3(-20.0f, 0.0f, -20.0f);
	// 区画4
	auto* store4 = Instantiate(spawnerPrefab);
	store4->AddComponent<Convenience>();
	store4->SetTag("Convenience");
	store4->transform().position() = Vector3(20.0f, 0.0f, -20.0f);

	
}
