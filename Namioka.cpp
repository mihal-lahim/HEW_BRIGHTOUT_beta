#include "Namioka.h"
#include "PlayerPrefab.h"
#include "EnemyPrefab.h"
#include "GameObject.h"
#include "DebugCamera.h"
#include "Texture.h"
#include "PowerPlant.h"
#include "Road.h"
#include "House.h"
#include "Convenience.h"
#include "Mansion01.h"
#include "Mansion02.h"
#include "Karaoke.h"
#include "Apartment.h"
#include "Clocktower.h"
#include "Wacdonald.h"
#include "Dentyuu.h"
#include "SpecialDentyuu.h"
#include "SpecialDensen.h"
#include "GameClearChecker.h"
#include "PoleManager.h"
#include "Pole.h"
#include "PowerLine.h"

	using namespace DirectX;

void Namioka::Initialize()
{
	// PoleManager
	GameObject* poleManagerObject = CreateGameObject();
	auto* poleManager = poleManagerObject->AddComponent<PoleManager>();

	//Player
	PlayerPrefab playerPrefab{};
	Instantiate(playerPrefab)->transform().position() += Vector3(0.0f, 0.0f, 10.0f);

	//Enemy
	EnemyPrefab enemyPrefab{};
	auto* enemy = Instantiate(enemyPrefab);
	enemy->transform().position() = Vector3(10.0f, 0.0f, 0.0f);

	////Ground
	//ModelPrefab cubePrefab{ "model/cube.glb" };
	//GameObject* floor = Instantiate(cubePrefab);
	//floor->transform().scale() = { 50.0f, 1.0f, 50.0f };
	//floor->transform().position() = { 0.0f, -1.0f, 0.0f };
	//BoxColliderDesc floorShapeDesc{};
	//floor->AddComponent<ColliderShape>(floorShapeDesc);
	//PhysicsBodyDesc floorBodyDesc{};
	//floorBodyDesc.Type = BodyType::STATIC;
	//floor->AddComponent<PhysicsBody>(floorBodyDesc);

	//Road
	ModelPrefab roadPrefab{ "model/intersection.glb" };
	std::vector<Vector3> roadPositions =
	{
		{    0.0f, 0.0f,    0.0f},   //‡@
		{    0.0f, 0.0f,  -60.0f},   //‡A
		{  -60.0f, 0.0f,  -60.0f},   //‡B
		{  -60.0f, 0.0f, -120.0f},   //‡C
		{    0.0f, 0.0f, -120.0f},   //‡D
		{ -120.0f, 0.0f, -120.0f},   //‡E
		{ -120.0f, 0.0f, -180.0f},   //‡F
		{ -120.0f, 0.0f, -240.0f},   //‡G
		{ -180.0f, 0.0f, -240.0f},   //‡H
	};
	for (const auto& pos : roadPositions)
	{
		GameObject* road = Instantiate(roadPrefab);
		road->AddComponent<Road>();

		road->transform().scale() = { 1.0f, 1.0f, 1.0f };
		road->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 40.0f;
		shapeDesc.SizeY = 2.0f;
		shapeDesc.SizeZ = 40.0f;
		road->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		road->AddComponent<PhysicsBody>(bodyDesc);
	}

	//House
	ModelPrefab housePrefab{ "model/house.glb" };
	std::vector<Vector3> housePositions =
	{
		//‡@
		{  -6.0f, 0.0f,  -6.0f },
		{ -18.0f, 0.0f,  -6.0f },
		{  -6.0f, 0.0f, -18.0f },
		{ -18.0f, 0.0f, -18.0f },

		//‡A
		{  -6.0f, 0.0f, -66.0f },
		{ -18.0f, 0.0f, -66.0f },
		{  -6.0f, 0.0f, -78.0f },
		{ -18.0f, 0.0f, -78.0f },

		//‡B
		{ -66.0f, 0.0f, -66.0f },
		{ -78.0f, 0.0f, -66.0f },
		{ -66.0f, 0.0f, -78.0f },
		{ -78.0f, 0.0f, -78.0f },

		//‡C
		{ -66.0f, 0.0f, -126.0f },
		{ -78.0f, 0.0f, -126.0f },
		{ -66.0f, 0.0f, -138.0f },
		{ -78.0f, 0.0f, -138.0f },

		//‡D
		{  -6.0f, 0.0f, -126.0f },
		{ -18.0f, 0.0f, -126.0f },
		{  -6.0f, 0.0f, -138.0f },
		{ -18.0f, 0.0f, -138.0f },

		//‡E
		{ -126.0f, 0.0f, -126.0f },
		{ -138.0f, 0.0f, -126.0f },
		{ -126.0f, 0.0f, -138.0f },
		{ -138.0f, 0.0f, -138.0f },

		//‡F
		{ -126.0f, 0.0f, -186.0f },
		{ -138.0f, 0.0f, -186.0f },
		{ -126.0f, 0.0f, -198.0f },
		{ -138.0f, 0.0f, -198.0f },

		//‡G
		{ -126.0f, 0.0f, -246.0f },
		{ -138.0f, 0.0f, -246.0f },
		{ -126.0f, 0.0f, -258.0f },
		{ -138.0f, 0.0f, -258.0f },

		//‡H
		{ -186.0f, 0.0f, -246.0f },
		{ -198.0f, 0.0f, -246.0f },
		{ -186.0f, 0.0f, -258.0f },
		{ -198.0f, 0.0f, -258.0f },
	};
	for (const auto& pos : housePositions)
	{
		GameObject* house = Instantiate(housePrefab);
		house->AddComponent<House>();

		house->transform().scale() = { 1.0f, 1.0f, 1.0f };
		house->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 2.0f;
		shapeDesc.SizeY = 2.0f;
		shapeDesc.SizeZ = 2.0f;
		house->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		house->AddComponent<PhysicsBody>(bodyDesc);
	}

	//Convenience
	ModelPrefab conveniencePrefab{ "model/convenience.glb" };
	std::vector<Vector3> conveniencePositions =
	{
		//‡@
		{ 6.0f, 0.0f,   6.0f },
		{ 6.0f, 0.0f, -16.0f },

		//‡A
		{ 6.0f, 0.0f, -54.0f },
		{ 6.0f, 0.0f, -76.0f },

		//‡B
		{ -54.0f, 0.0f, -54.0f },
		{ -54.0f, 0.0f, -76.0f },

		//‡C
		{ -54.0f, 0.0f, -114.0f },
		{ -54.0f, 0.0f, -136.0f },

		//‡D
		{ 6.0f, 0.0f, -114.0f },
		{ 6.0f, 0.0f, -136.0f },

		//‡E
		{ -114.0f, 0.0f, -114.0f },
		{ -114.0f, 0.0f, -136.0f },

		//‡F
		{ -114.0f, 0.0f, -174.0f },
		{ -114.0f, 0.0f, -196.0f },

		//‡G
		{ -114.0f, 0.0f, -234.0f },
		{ -114.0f, 0.0f, -256.0f },

		//‡H
		{ -174.0f, 0.0f, -234.0f },
		{ -174.0f, 0.0f, -256.0f },
	};
	for (const auto& pos : conveniencePositions)
	{
		GameObject* convenience = Instantiate(conveniencePrefab);
		convenience->AddComponent<Convenience>();

		convenience->transform().scale() = { 1.0f, 1.0f, 1.0f };
		convenience->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 4.0f;
		shapeDesc.SizeY = 4.0f;
		shapeDesc.SizeZ = 4.0f;
		convenience->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		convenience->AddComponent<PhysicsBody>(bodyDesc);
	}

	//Mansion01
	ModelPrefab mansion01Prefab{ "model/mansion01.glb" };
	std::vector<Vector3> mansion01Positions =
	{
		//‡@
		{  -8.0f, 0.0f, 18.0f },
		{ -16.0f, 0.0f,  8.0f },
		{   6.0f, 0.0f, -6.0f },

		//‡A
		{  -8.0f, 0.0f, -42.0f },
		{ -16.0f, 0.0f, -52.0f },
		{   6.0f, 0.0f, -66.0f },

		//‡B
		{ -68.0f, 0.0f, -42.0f },
		{ -76.0f, 0.0f, -52.0f },
		{ -54.0f, 0.0f, -66.0f },

		//‡C
		{ -68.0f, 0.0f, -102.0f },
		{ -76.0f, 0.0f, -112.0f },
		{ -54.0f, 0.0f, -126.0f },

		//‡D
		{  -8.0f, 0.0f, -102.0f },
		{ -16.0f, 0.0f, -112.0f },
		{   6.0f, 0.0f, -126.0f },

		//‡E
		{ -128.0f, 0.0f, -102.0f },
		{ -136.0f, 0.0f, -112.0f },
		{ -114.0f, 0.0f, -126.0f },

		//‡F
		{ -128.0f, 0.0f, -162.0f },
		{ -136.0f, 0.0f, -172.0f },
		{ -114.0f, 0.0f, -186.0f },

		//‡G
		{ -128.0f, 0.0f, -222.0f },
		{ -136.0f, 0.0f, -232.0f },
		{ -114.0f, 0.0f, -246.0f },

		//‡H
		{ -188.0f, 0.0f, -222.0f },
		{ -196.0f, 0.0f, -232.0f },
		{ -174.0f, 0.0f, -246.0f },
	};
	for (const auto& pos : mansion01Positions)
	{
		GameObject* mansion01 = Instantiate(mansion01Prefab);
		mansion01->AddComponent<Mansion01>();

		mansion01->transform().scale() = { 1.0f, 1.0f, 1.0f };
		mansion01->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 5.0f;
		shapeDesc.SizeY = 5.0f;
		shapeDesc.SizeZ = 5.0f;
		mansion01->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		mansion01->AddComponent<PhysicsBody>(bodyDesc);
	}

	//Mansion02
	ModelPrefab mansion02Prefab{ "model/mansion02.glb" };
	std::vector<Vector3> mansion02Positions =
	{
		//‡@
		{ 8.0f, 0.0f, 18.0f },

		//‡A
		{ 8.0f, 0.0f, -42.0f },

		//‡B
		{ -52.0f, 0.0f, -42.0f },

		//‡C
		{ -52.0f, 0.0f, -102.0f },

		//‡D
		{ 8.0f, 0.0f, -102.0f },

		//‡E
		{ -112.0f, 0.0f, -102.0f },

		//‡F
		{ -112.0f, 0.0f, -162.0f },

		//‡G
		{ -112.0f, 0.0f, -222.0f },

		//‡H
		{ -172.0f, 0.0f, -222.0f },
	};
	for (const auto& pos : mansion02Positions)
	{
		GameObject* mansion02 = Instantiate(mansion02Prefab);
		mansion02->AddComponent<Mansion02>();

		mansion02->transform().scale() = { 1.0f, 1.0f, 1.0f };
		mansion02->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 2.0f;
		shapeDesc.SizeY = 2.0f;
		shapeDesc.SizeZ = 2.0f;
		mansion02->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		mansion02->AddComponent<PhysicsBody>(bodyDesc);
	}

	//karaoke
	ModelPrefab karaokePrefab{ "model/karaoke.glb" };
	std::vector<Vector3> karaokePositions =
	{
		//‡@
		{ 16.0f, 0.0f, 8.0f },

		//‡A
		{ 16.0f, 0.0f, -52.0f },

		//‡B
		{ -44.0f, 0.0f, -52.0f },

		//‡C
		{ -44.0f, 0.0f, -112.0f },

		//‡D
		{ 16.0f, 0.0f, -112.0f },

		//‡E
		{ -104.0f, 0.0f, -112.0f },

		//‡F
		{ -104.0f, 0.0f, -172.0f },

		//‡G
		{ -104.0f, 0.0f, -232.0f },

		//‡H
		{ -164.0f, 0.0f, -232.0f },
	};
	for (const auto& pos : karaokePositions)
	{
		GameObject* karaoke = Instantiate(karaokePrefab);
		karaoke->AddComponent<Karaoke>();

		karaoke->transform().scale() = { 1.0f, 1.0f, 1.0f };
		karaoke->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 3.0f;
		shapeDesc.SizeY = 3.0f;
		shapeDesc.SizeZ = 3.0f;
		karaoke->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		karaoke->AddComponent<PhysicsBody>(bodyDesc);
	}

	//apartment
	ModelPrefab apartmentPrefab{ "model/apartment.glb" };
	std::vector<Vector3> apartmentPositions =
	{
		//‡@
		{ -8.0f, 0.0f,   8.0f },
		{ 16.0f, 0.0f, -16.0f },

		//‡A
		{ -8.0f, 0.0f, -52.0f },
		{ 16.0f, 0.0f, -76.0f },

		//‡B
		{ -68.0f, 0.0f, -52.0f },
		{ -44.0f, 0.0f, -76.0f },

		//‡C
		{ -68.0f, 0.0f, -112.0f },
		{ -44.0f, 0.0f, -136.0f },

		//‡D
		{ -8.0f, 0.0f, -112.0f },
		{ 16.0f, 0.0f, -136.0f },

		//‡E
		{ -128.0f, 0.0f, -112.0f },
		{ -104.0f, 0.0f, -136.0f },

		//‡F
		{ -128.0f, 0.0f, -172.0f },
		{ -104.0f, 0.0f, -196.0f },

		//‡G
		{ -128.0f, 0.0f, -232.0f },
		{ -104.0f, 0.0f, -256.0f },

		//‡H
		{ -188.0f, 0.0f, -232.0f },
		{ -164.0f, 0.0f, -256.0f },
	};
	for (const auto& pos : apartmentPositions)
	{
		GameObject* apartment = Instantiate(apartmentPrefab);
		apartment->AddComponent<Apartment>();

		apartment->transform().scale() = { 1.0f, 1.0f, 1.0f };
		apartment->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 8.0f;
		shapeDesc.SizeY = 16.0f;
		shapeDesc.SizeZ = 8.0f;
		apartment->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		apartment->AddComponent<PhysicsBody>(bodyDesc);
	}


	//clocktower
	ModelPrefab clocktowerPrefab{ "model/clocktower01.glb" };
	std::vector<Vector3> clocktowerPositions =
	{
		//‡@
		{ -11.0f, 0.0f, -11.0f },

		//‡A
		{ -11.0f, 0.0f, -71.0f },

		//‡B
		{ -71.0f, 0.0f, -71.0f },

		//‡C
		{ -71.0f, 0.0f, -131.0f },

		//‡D
		{ -11.0f, 0.0f, -131.0f },

		//‡E
		{ -131.0f, 0.0f, -131.0f },

		//‡F
		{ -131.0f, 0.0f, -191.0f },

		//‡G
		{ -131.0f, 0.0f, -251.0f },

		//‡H
		{ -191.0f, 0.0f, -251.0f },
	};
	for (const auto& pos : clocktowerPositions)
	{
		GameObject* clocktower = Instantiate(clocktowerPrefab);
		clocktower->AddComponent<Clocktower>();

		clocktower->transform().scale() = { 0.5f, 0.5f, 0.5f };
		clocktower->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 4.0f;
		shapeDesc.SizeY = 4.0f;
		shapeDesc.SizeZ = 4.0f;
		clocktower->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		clocktower->AddComponent<PhysicsBody>(bodyDesc);
	}

	//wacdonald
	ModelPrefab wacdonaldPrefab{ "model/wacdonald.glb" };
	std::vector<Vector3> wacdonaldPositions =
	{
		//‡@
		{ 14.0f, 0.0f, -6.0f },

		//‡A
		{ 14.0f, 0.0f, -66.0f },

		//‡B
		{ -46.0f, 0.0f, -66.0f },

		//‡C
		{ -46.0f, 0.0f, -126.0f },

		//‡D
		{ 14.0f, 0.0f, -126.0f },

		//‡E
		{ -106.0f, 0.0f, -126.0f },

		//‡F
		{ -106.0f, 0.0f, -186.0f },

		//‡G
		{ -106.0f, 0.0f, -246.0f },

		//‡H
		{ -166.0f, 0.0f, -246.0f },
	};
	for (const auto& pos : wacdonaldPositions)
	{
		GameObject* wacdonald = Instantiate(wacdonaldPrefab);
		wacdonald->AddComponent<Wacdonald>();

		wacdonald->transform().scale() = { 1.0f, 1.0f, 1.0f };
		wacdonald->transform().position() = pos;
		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(0.0f),     // X²
			XMConvertToRadians(180.0f),   // Y²
			XMConvertToRadians(0.0f)      // Z²
		);

		wacdonald->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 4.0f;
		shapeDesc.SizeY = 4.0f;
		shapeDesc.SizeZ = 4.0f;
		wacdonald->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		wacdonald->AddComponent<PhysicsBody>(bodyDesc);
	}

	//powerplant
	ModelPrefab powerplantPrefab{ "model/powerplant.glb" };
	ModelPrefab powerplant_brokenPrefab{ "model/powerplant_broken.glb" };
	std::vector<Vector3> powerplantPositions =
	{
		//‡@
		{ 0.0f, 0.0f, 0.0f },

		//‡A
		{ 0.0f, 0.0f, -60.0f },

		//‡B
		{ -60.0f, 0.0f, -60.0f },

		//‡C
		{ -60.0f, 0.0f, -120.0f },

		//‡D
		{ 0.0f, 0.0f, -120.0f },

		//‡E
		{ -120.0f, 0.0f, -120.0f },

		//‡F
		{ -120.0f, 0.0f, -180.0f },

		//‡G
		{ -120.0f, 0.0f, -240.0f},

		//‡H
		{ -180.0f, 0.0f, -240.0f },
	};
	for (const auto& pos : powerplantPositions)
	{
		// eƒIƒuƒWƒFƒNƒgiPowerPlantƒRƒ“ƒ|[ƒlƒ“ƒg•t‚«j
		GameObject* plantObj = CreateGameObject();
		plantObj->SetName("PowerPlant");
		plantObj->SetTag("PowerPlant");
		plantObj->transform().position() = pos;
		auto* plant = plantObj->AddComponent<PowerPlant>(10.0f);

		// “–‚½‚è”»’è‚ÍeƒIƒuƒWƒFƒNƒg‚Éİ’è
		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 6.0f;
		shapeDesc.SizeY = 6.0f;
		shapeDesc.SizeZ = 6.0f;
		plantObj->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		plantObj->AddComponent<PhysicsBody>(bodyDesc);

		// •œ‹ŒÏ‚İƒ‚ƒfƒ‹iStart()‚Å”ñ•\¦‚É‚·‚éj
		GameObject* restored = Instantiate(powerplantPrefab);
		restored->transform().position() = pos;

		// ŒÌáƒ‚ƒfƒ‹i‰Šú‚Í•\¦j
		GameObject* broken = Instantiate(powerplant_brokenPrefab);
		broken->transform().position() = pos;

		// PowerPlant‚Éƒ‚ƒfƒ‹QÆ‚ğİ’è
		plant->restoredModel = restored;
		plant->brokenModel = broken;
	}

	//dentyuu
	ModelPrefab dentyuuPrefab{ "model/dentyuu.glb" };
	std::vector<Vector3> dentyuuPositions =
	{
		//‡@
		{   3.0f, 2.0f,  10.0f },
		{   3.0f, 2.0f,  20.0f },
		{  -3.0f, 2.0f,  10.0f },
		{  -3.0f, 2.0f,  20.0f },
		{   3.0f, 2.0f, -10.0f },
		{   3.0f, 2.0f, -20.0f },
		{  -3.0f, 2.0f, -10.0f },
		{  -3.0f, 2.0f, -20.0f },
		{   9.0f, 2.0f,   3.0f },
		{  18.0f, 2.0f,   3.0f },
		{   9.0f, 2.0f,  -3.0f },
		{  18.0f, 2.0f,  -3.0f },
		{  -9.0f, 2.0f,   3.0f },
		{ -18.0f, 2.0f,   3.0f },
		{  -9.0f, 2.0f,  -3.0f },
		{ -18.0f, 2.0f,  -3.0f },

		//‡A
		{   3.0f, 2.0f, -50.0f },
		{   3.0f, 2.0f, -40.0f },
		{  -3.0f, 2.0f, -50.0f },
		{  -3.0f, 2.0f, -40.0f },
		{   3.0f, 2.0f, -70.0f },
		{   3.0f, 2.0f, -80.0f },
		{  -3.0f, 2.0f, -70.0f },
		{  -3.0f, 2.0f, -80.0f },
		{   9.0f, 2.0f, -57.0f },
		{  18.0f, 2.0f, -57.0f },
		{   9.0f, 2.0f, -63.0f },
		{  18.0f, 2.0f, -63.0f },
		{  -9.0f, 2.0f, -57.0f },
		{ -18.0f, 2.0f, -57.0f },
		{  -9.0f, 2.0f, -63.0f },
		{ -18.0f, 2.0f, -63.0f },

		//‡B
		{ -57.0f, 2.0f, -50.0f },
		{ -57.0f, 2.0f, -40.0f },
		{ -63.0f, 2.0f, -50.0f },
		{ -63.0f, 2.0f, -40.0f },
		{ -57.0f, 2.0f, -70.0f },
		{ -57.0f, 2.0f, -80.0f },
		{ -63.0f, 2.0f, -70.0f },
		{ -63.0f, 2.0f, -80.0f },
		{ -51.0f, 2.0f, -57.0f },
		{ -42.0f, 2.0f, -57.0f },
		{ -51.0f, 2.0f, -63.0f },
		{ -42.0f, 2.0f, -63.0f },
		{ -69.0f, 2.0f, -57.0f },
		{ -78.0f, 2.0f, -57.0f },
		{ -69.0f, 2.0f, -63.0f },
		{ -78.0f, 2.0f, -63.0f },

		//‡C
		{ -57.0f, 2.0f, -110.0f },
		{ -57.0f, 2.0f, -100.0f },
		{ -63.0f, 2.0f, -110.0f },
		{ -63.0f, 2.0f, -100.0f },
		{ -57.0f, 2.0f, -130.0f },
		{ -57.0f, 2.0f, -140.0f },
		{ -63.0f, 2.0f, -130.0f },
		{ -63.0f, 2.0f, -140.0f },
		{ -51.0f, 2.0f, -117.0f },
		{ -42.0f, 2.0f, -117.0f },
		{ -51.0f, 2.0f, -123.0f },
		{ -42.0f, 2.0f, -123.0f },
		{ -69.0f, 2.0f, -117.0f },
		{ -78.0f, 2.0f, -117.0f },
		{ -69.0f, 2.0f, -123.0f },
		{ -78.0f, 2.0f, -123.0f },

		//‡D
		{   3.0f, 2.0f, -110.0f },
		{   3.0f, 2.0f, -100.0f },
		{  -3.0f, 2.0f, -110.0f },
		{  -3.0f, 2.0f, -100.0f },
		{   3.0f, 2.0f, -130.0f },
		{   3.0f, 2.0f, -140.0f },
		{  -3.0f, 2.0f, -130.0f },
		{  -3.0f, 2.0f, -140.0f },
		{   9.0f, 2.0f, -117.0f },
		{  18.0f, 2.0f, -117.0f },
		{   9.0f, 2.0f, -123.0f },
		{  18.0f, 2.0f, -123.0f },
		{  -9.0f, 2.0f, -117.0f },
		{ -18.0f, 2.0f, -117.0f },
		{  -9.0f, 2.0f, -123.0f },
		{ -18.0f, 2.0f, -123.0f },

		//‡E
		{ -117.0f, 2.0f, -110.0f },
		{ -117.0f, 2.0f, -100.0f },
		{ -123.0f, 2.0f, -110.0f },
		{ -123.0f, 2.0f, -100.0f },
		{ -117.0f, 2.0f, -130.0f },
		{ -117.0f, 2.0f, -140.0f },
		{ -123.0f, 2.0f, -130.0f },
		{ -123.0f, 2.0f, -140.0f },
		{ -111.0f, 2.0f, -117.0f },
		{ -102.0f, 2.0f, -117.0f },
		{ -111.0f, 2.0f, -123.0f },
		{ -102.0f, 2.0f, -123.0f },
		{ -129.0f, 2.0f, -117.0f },
		{ -138.0f, 2.0f, -117.0f },
		{ -129.0f, 2.0f, -123.0f },
		{ -138.0f, 2.0f, -123.0f },

		//‡F
		{ -117.0f, 2.0f, -170.0f },
		{ -117.0f, 2.0f, -160.0f },
		{ -123.0f, 2.0f, -170.0f },
		{ -123.0f, 2.0f, -160.0f },
		{ -117.0f, 2.0f, -190.0f },
		{ -117.0f, 2.0f, -200.0f },
		{ -123.0f, 2.0f, -190.0f },
		{ -123.0f, 2.0f, -200.0f },
		{ -111.0f, 2.0f, -177.0f },
		{ -102.0f, 2.0f, -177.0f },
		{ -111.0f, 2.0f, -183.0f },
		{ -102.0f, 2.0f, -183.0f },
		{ -129.0f, 2.0f, -177.0f },
		{ -138.0f, 2.0f, -177.0f },
		{ -129.0f, 2.0f, -183.0f },
		{ -138.0f, 2.0f, -183.0f },

		//‡G
		{ -117.0f, 2.0f, -230.0f },
		{ -117.0f, 2.0f, -220.0f },
		{ -123.0f, 2.0f, -230.0f },
		{ -123.0f, 2.0f, -220.0f },
		{ -117.0f, 2.0f, -250.0f },
		{ -117.0f, 2.0f, -260.0f },
		{ -123.0f, 2.0f, -250.0f },
		{ -123.0f, 2.0f, -260.0f },
		{ -111.0f, 2.0f, -237.0f },
		{ -102.0f, 2.0f, -237.0f },
		{ -111.0f, 2.0f, -243.0f },
		{ -102.0f, 2.0f, -243.0f },
		{ -129.0f, 2.0f, -237.0f },
		{ -138.0f, 2.0f, -237.0f },
		{ -129.0f, 2.0f, -243.0f },
		{ -138.0f, 2.0f, -243.0f },

		//‡H
		{ -177.0f, 2.0f, -230.0f },
		{ -177.0f, 2.0f, -220.0f },
		{ -183.0f, 2.0f, -230.0f },
		{ -183.0f, 2.0f, -220.0f },
		{ -177.0f, 2.0f, -250.0f },
		{ -177.0f, 2.0f, -260.0f },
		{ -183.0f, 2.0f, -250.0f },
		{ -183.0f, 2.0f, -260.0f },
		{ -171.0f, 2.0f, -237.0f },
		{ -162.0f, 2.0f, -237.0f },
		{ -171.0f, 2.0f, -243.0f },
		{ -162.0f, 2.0f, -243.0f },
		{ -189.0f, 2.0f, -237.0f },
		{ -198.0f, 2.0f, -237.0f },
		{ -189.0f, 2.0f, -243.0f },
		{ -198.0f, 2.0f, -243.0f },
	};
	for (const auto& pos : dentyuuPositions)
	{
		GameObject* dentyuu = Instantiate(dentyuuPrefab);
		dentyuu->AddComponent<Dentyuu>();

		dentyuu->transform().scale() = { 10.0f, 8.0f, 10.0f };
		dentyuu->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 0.1f;
		shapeDesc.SizeY = 0.5f;
		shapeDesc.SizeZ = 0.1f;
		dentyuu->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		dentyuu->AddComponent<PhysicsBody>(bodyDesc);
	}

	//Special_Dentyuu
	ModelPrefab spedentyuPrefab{ "model/special_dentyu.glb" };
	std::vector<Vector3> spedentyuPositions =
	{
		//‡@¨‡A
		{ 0.0f, 0.0f, -18.0f},

		//‡A¨‡@
		{ 0.0f, 0.0f, -42.0f },

		//‡A¨‡B
		{ -18.0f, 0.0f, -60.0f },

		//‡B¨‡A
		{ -42.0f, 0.0f, -60.0f },

		//‡B¨‡C
		{ -60.0f, 0.0f, -78.0f },

		//‡C¨‡B
		{ -60.0f, 0.0f, -102.0f },

		//‡C¨‡D
		{ -42.0f, 0.0f, -120.0f },

		//‡D¨‡C
		{ -18.0f, 0.0f, -120.0f },

		//‡C¨‡E
		{ -102.0f, 0.0f, -120.0f },

		//‡E¨‡C
		{ -78.0f, 0.0f, -120.0f },

		//‡E¨‡F
		{ -120.0f, 0.0f, -138.0f },

		//‡F¨‡E
		{ -120.0f, 0.0f, -162.0f },

		//‡F¨‡G
		{ -120.0f, 0.0f, -198.0f },

		//‡G¨‡F
		{ -120.0f, 0.0f, -222.0f },

		//‡G¨‡H
		{ -162.0f, 0.0f, -240.0f },

		//‡H¨‡G
		{ -138.0f, 0.0f, -240.0f },


	};
	std::vector<PoleID> specialPoleIDs;
	specialPoleIDs.reserve(spedentyuPositions.size());
	for (const auto& pos : spedentyuPositions)
	{
		GameObject* spedentyuu = Instantiate(spedentyuPrefab);
		spedentyuu->AddComponent<SpecialDentyuu>();
		auto* pole = spedentyuu->AddComponent<Pole>();
		specialPoleIDs.push_back(poleManager->RegisterPole(pole));

		spedentyuu->transform().scale() = { 0.1f, 0.1f, 0.1f };
		spedentyuu->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 1.0f;
		shapeDesc.SizeY = 1.0f;
		shapeDesc.SizeZ = 1.0f;
		spedentyuu->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		spedentyuu->AddComponent<PhysicsBody>(bodyDesc);
	}

	//Special_Densen
	struct DensenData
	{
		Vector3 position;
		float yRotation; // Y²‰ñ“]i“xj
	};
	ModelPrefab spedensenPrefab{ "model/special_densen.glb" };
	std::vector<DensenData> spedensenList =
	{
		// ‡@¨‡A
		{ { 0.0f, 5.0f, -18.0f },  0.0f },

		// ‡A¨‡B
		{ { -18.0f, 5.0f, -60.0f },  90.0f },

		// ‡B¨‡C
		{ { -60.0f, 5.0f, -78.0f },  0.0f },

		// ‡C¨‡D
		{ { -18.0f, 5.0f, -120.0f },  90.0f },

		// ‡C¨‡E
		{ { -78.0f, 5.0f, -120.0f },  90.0f },

		// ‡E¨‡F
		{ { -120.0f, 5.0f, -138.0f },  0.0f },

		// ‡F¨‡G
		{ { -120.0f, 5.0f, -198.0f },  0.0f },

		// ‡G¨‡H
		{ { -138.0f, 5.0f, -240.0f },  90.0f },
	};

	std::vector<std::pair<size_t, size_t>> specialLinePolePairs =
	{
		{ 0, 1 },
		{ 2, 3 },
		{ 4, 5 },
		{ 6, 7 },
		{ 8, 9 },
		{ 10, 11 },
		{ 12, 13 },
		{ 14, 15 }
	};
	for (size_t index = 0; index < spedensenList.size(); ++index)
	{
		const auto& data = spedensenList.at(index);
		GameObject* spedentyuu = Instantiate(spedensenPrefab);
		spedentyuu->AddComponent<SpecialDensen>();
		if (index < specialLinePolePairs.size() && specialPoleIDs.size() > specialLinePolePairs.at(index).second)
		{
			auto [startIndex, endIndex] = specialLinePolePairs.at(index);
			auto* line = spedentyuu->AddComponent<PowerLine>(specialPoleIDs.at(startIndex), specialPoleIDs.at(endIndex));
			poleManager->RegisterPowerLine(line);
		}

		spedentyuu->transform().scale() = { 0.2f,0.2f,0.18f };
		spedentyuu->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		spedentyuu->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 1.0f;
		shapeDesc.SizeY = 1.0f;
		shapeDesc.SizeZ = 1.0f;
		spedentyuu->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		spedentyuu->AddComponent<PhysicsBody>(bodyDesc);
	}

	// ‘S”­“dŠ‚Ì•œ‹Œƒ`ƒFƒbƒN—pGameObject‚ğì¬
	GameObject* checker = CreateGameObject();
	checker->AddComponent<GameClearChecker>();
}
