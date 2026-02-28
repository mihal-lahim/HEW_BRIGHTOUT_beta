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
#include "EnemySpawner.h"
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
#include "audio.h"
#include "PoleManager.h"
#include "Pole.h"
#include "PowerLine.h"
#include "RenderingSystem.h"
#include "UIQuad.h"
#include "UIDrawer.h"
#include "TimerUI.h"
#include "HPBarUI.h"
#include "MorphUI.h"
#include "BgmSwitcher.h"
#include "ScoreData.h"
#include "SkyDome.h"
#include "FadeController.h"

using namespace DirectX;

static int g_GameBgm{};
static int g_GameBgm2{};

void Namioka::Initialize()
{
	// フェードイン前に画面を黒くしておく（灰色が一瞬映るのを防ぐ）
	rendering().GetGraphicsDevice().SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// スコアデータリセット
	ScoreData::Instance().Reset();

	// オーディオ初期化
	InitAudio();
	PowerPlant::ResetAudioState();

	g_GameBgm = LoadAudio("sound/GameBGM_01.wav");

	//ラストスパート用BGM
	g_GameBgm2 = LoadAudio("sound/GameBGM_02.wav");

	// BGM再生（ループ）
	PlayAudio(g_GameBgm, true);
	SetAudioVolume(g_GameBgm, 0.2f);
	SetAudioVolume(g_GameBgm2, 0.5f);

	// スカイドーム
	ModelPrefab skyPrefab{ "model/sky.fbx" };
	GameObject* skyDome = Instantiate(skyPrefab);
	skyDome->AddComponent<SkyDome>();
	skyDome->transform().scale() = { 10.0f, 10.0f, 10.0f };
	skyDome->transform().position() = { 0.0f, 0.0f, 0.0f };

	//残り60秒になったらBGM切り替え
	GameObject* bgmObj = CreateGameObject();
	bgmObj->SetName("BgmSwitcher");
	auto* switcher = bgmObj->AddComponent<BgmSwitcher>();
	switcher->currentIndex = g_GameBgm;
	switcher->nextIndex = g_GameBgm2;
	switcher->switchAtSeconds = 60.0f;

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
  
	//Road
	ModelPrefab roadPrefab{ "model/intersection.glb" };
	std::vector<Vector3> roadPositions =
	{
		{    0.0f, 0.0f,    0.0f},   //①
		{    0.0f, 0.0f,  -60.0f},   //②
		{  -60.0f, 0.0f,  -60.0f},   //③
		{  -60.0f, 0.0f, -120.0f},   //④
		{    0.0f, 0.0f, -120.0f},   //⑤
		{ -120.0f, 0.0f, -120.0f},   //⑥
		{ -120.0f, 0.0f, -180.0f},   //⑦
		{ -120.0f, 0.0f, -240.0f},   //⑧
		{ -180.0f, 0.0f, -240.0f},   //⑨
	};
	for (const auto& pos : roadPositions)
	{
		GameObject* road = Instantiate(roadPrefab);
		road->AddComponent<Road>();

		road->transform().scale() = { 1.0f, 1.0f, 1.0f };
		road->transform().position() = pos;

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 40.0f;
		shapeDesc.SizeY = 1.0f;
		shapeDesc.SizeZ = 40.0f;
		road->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		road->AddComponent<PhysicsBody>(bodyDesc);
	}

	//House
	struct HouseData
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab housePrefab{ "model/house.glb" };
	std::vector<HouseData> houseList =
	{
		//①
		{ {  -6.0f, 0.0f,  -6.0f },   0.0f },
		{ { -18.0f, 0.0f,  -6.0f },	  0.0f },
		{ {  -6.0f, 0.0f, -18.0f },	  0.0f },
		{ { -18.0f, 0.0f, -18.0f },	  0.0f },
								
		//②					
		{ {  -6.0f, 0.0f, -66.0f },	  0.0f },
		{ { -18.0f, 0.0f, -66.0f },	  0.0f },
		{ {  -6.0f, 0.0f, -78.0f },	  0.0f },
		{ { -18.0f, 0.0f, -78.0f },	  0.0f },
		 					
		//③						
		{ { -66.0f, 0.0f, -66.0f },	  0.0f },
		{ { -78.0f, 0.0f, -66.0f },	  0.0f },
		{ { -66.0f, 0.0f, -78.0f },	  0.0f },
		{ { -78.0f, 0.0f, -78.0f },	  0.0f },
							
		//④						
		{ { -66.0f, 0.0f, -126.0f },  0.0f },
		{ { -78.0f, 0.0f, -126.0f },  0.0f },
		{ { -66.0f, 0.0f, -138.0f },  0.0f },
		{ { -78.0f, 0.0f, -138.0f },  0.0f },
		 						
		//⑤						
		{ {  -6.0f, 0.0f, -126.0f },  0.0f },
		{ { -18.0f, 0.0f, -126.0f },  0.0f },
		{ {  -6.0f, 0.0f, -138.0f },  0.0f },
		{ { -18.0f, 0.0f, -138.0f },  0.0f },
		 							
		//⑥						
		{ { -126.0f, 0.0f, -126.0f }, 0.0f },
		{ { -138.0f, 0.0f, -126.0f }, 0.0f },
		{ { -126.0f, 0.0f, -138.0f }, 0.0f },
		{ { -138.0f, 0.0f, -138.0f }, 0.0f },
								
		//⑦						
		{ { -126.0f, 0.0f, -186.0f }, 0.0f },
		{ { -138.0f, 0.0f, -186.0f }, 0.0f },
		{ { -126.0f, 0.0f, -198.0f }, 0.0f },
		{ { -138.0f, 0.0f, -198.0f }, 0.0f },
		 						
		//⑧						
		{ { -126.0f, 0.0f, -246.0f }, 0.0f },
		{ { -138.0f, 0.0f, -246.0f }, 0.0f },
		{ { -126.0f, 0.0f, -258.0f }, 0.0f },
		{ { -138.0f, 0.0f, -258.0f }, 0.0f },
		 							
		//⑨							
		{ { -186.0f, 0.0f, -246.0f }, 0.0f },
		{ { -198.0f, 0.0f, -246.0f }, 0.0f },
		{ { -186.0f, 0.0f, -258.0f }, 0.0f },
		{ { -198.0f, 0.0f, -258.0f }, 0.0f },
	};
	for (const auto& data : houseList)
	{
		GameObject* house = Instantiate(housePrefab);
		house->AddComponent<House>();

		house->transform().scale() = { 1.3f, 1.3f, 1.3f };
		house->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		house->transform().rotation().FromXMVECTOR(q);


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
	struct ConvenienceData
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab conveniencePrefab{ "model/convenience.glb" };
	std::vector<ConvenienceData> convenienceList =
	{
		//①
		{ { 6.0f, 0.0f,   6.0f },     0.0f },
		{ { 6.0f, 0.0f, -16.0f },	  0.0f },
	 							
		//②						
		{ { 6.0f, 0.0f, -54.0f },	  0.0f },
		{ { 6.0f, 0.0f, -76.0f },	  0.0f },
	 								
		//③						
		{ { -54.0f, 0.0f, -54.0f },	  0.0f },
		{ { -54.0f, 0.0f, -76.0f },	  0.0f },
								
		//④						
		{ { -54.0f, 0.0f, -114.0f },  0.0f },
		{ { -54.0f, 0.0f, -136.0f },  0.0f },
								
		//⑤							
		{ { 6.0f, 0.0f, -114.0f },	  0.0f },
		{ { 6.0f, 0.0f, -136.0f },	  0.0f },
									
		//⑥						
		{ { -114.0f, 0.0f, -114.0f }, 0.0f },
		{ { -114.0f, 0.0f, -136.0f }, 0.0f },
		 						
		//⑦						
		{ { -114.0f, 0.0f, -174.0f }, 0.0f },
		{ { -114.0f, 0.0f, -196.0f }, 0.0f },
									
		//⑧							
		{ { -114.0f, 0.0f, -234.0f }, 0.0f },
		{ { -114.0f, 0.0f, -256.0f }, 0.0f },
		 							
		//⑨						
		{ { -174.0f, 0.0f, -234.0f }, 0.0f },
		{ { -174.0f, 0.0f, -256.0f }, 0.0f },
	};
	for (const auto& data : convenienceList)
	{
		GameObject* convenience = Instantiate(conveniencePrefab);
		convenience->AddComponent<Convenience>();
		auto* spawner = convenience->AddComponent<EnemySpawner>();
		spawner->InitialInterval = 5.0f;
		spawner->MinInterval = 1.0f;
		spawner->IntervalDecreaseRate = 0.1f;
		spawner->MaxEnemies = 10;
		spawner->SpawnRadius = 3.0f;

		convenience->transform().scale() = { 1.0f, 1.0f, 1.0f };
		convenience->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		convenience->transform().rotation().FromXMVECTOR(q);

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
	struct Mansion01Data
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab mansion01Prefab{ "model/mansion01.glb" };
	std::vector<Mansion01Data> mansion01List =
	{
		//①
		{ {  -8.0f, 0.0f, 18.0f },    0.0f },
		{ { -16.0f, 0.0f,  8.0f },	  0.0f },
		{ {   6.0f, 0.0f, -6.0f },	  0.0f },
		 						
		//②						
		{ {  -8.0f, 0.0f, -42.0f },	  0.0f },
		{ { -16.0f, 0.0f, -52.0f },	  0.0f },
		{ {   6.0f, 0.0f, -66.0f },	  0.0f },
		 						
		//③						
		{ { -68.0f, 0.0f, -42.0f },	  0.0f },
		{ { -76.0f, 0.0f, -52.0f },	  0.0f },
		{ { -54.0f, 0.0f, -66.0f },	  0.0f },
		 						
		//④					
		{ { -68.0f, 0.0f, -102.0f },  0.0f },
		{ { -76.0f, 0.0f, -112.0f },  0.0f },
		{ { -54.0f, 0.0f, -126.0f },  0.0f },
		 							
		//⑤							
		{ {  -8.0f, 0.0f, -102.0f },  0.0f },
		{ { -16.0f, 0.0f, -112.0f },  0.0f },
		{ {   6.0f, 0.0f, -126.0f },  0.0f },
								
		//⑥						
		{ { -128.0f, 0.0f, -102.0f }, 0.0f },
		{ { -136.0f, 0.0f, -112.0f }, 0.0f },
		{ { -114.0f, 0.0f, -126.0f }, 0.0f },
		 							
		//⑦						
		{ { -128.0f, 0.0f, -162.0f }, 0.0f },
		{ { -136.0f, 0.0f, -172.0f }, 0.0f },
		{ { -114.0f, 0.0f, -186.0f }, 0.0f },
		 						
		//⑧						
		{ { -128.0f, 0.0f, -222.0f }, 0.0f },
		{ { -136.0f, 0.0f, -232.0f }, 0.0f },
		{ { -114.0f, 0.0f, -246.0f }, 0.0f },
									
		//⑨						
		{ { -188.0f, 0.0f, -222.0f }, 0.0f },
		{ { -196.0f, 0.0f, -232.0f }, 0.0f },
		{ { -174.0f, 0.0f, -246.0f }, 0.0f },
	};
	for (const auto& data : mansion01List)
	{
		GameObject* mansion01 = Instantiate(mansion01Prefab);
		mansion01->AddComponent<Mansion01>();

		mansion01->transform().scale() = { 1.0f, 1.0f, 1.0f };
		mansion01->transform().position() = data.position;
		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		mansion01->transform().rotation().FromXMVECTOR(q);

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
	struct Mansion02Data
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab mansion02Prefab{ "model/mansion02.glb" };
	std::vector<Mansion02Data> mansion02List =
	{
		//①
		{ { 8.0f, 0.0f, 18.0f },      0.0f },
	 								 
		//②							
		{ { 8.0f, 0.0f, -42.0f },	  0.0f },
									
		//③						
		{ { -52.0f, 0.0f, -42.0f },	  0.0f },
		 							
		//④						
		{ { -52.0f, 0.0f, -102.0f },  0.0f },
									
		//⑤						
		{ { 8.0f, 0.0f, -102.0f },	  0.0f },
		 						
		//⑥						
		{ { -112.0f, 0.0f, -102.0f }, 0.0f },
		 							
		//⑦						
		{ { -112.0f, 0.0f, -162.0f }, 0.0f },
								
		//⑧						
		{ { -112.0f, 0.0f, -222.0f }, 0.0f },
								
		//⑨						
		{ { -172.0f, 0.0f, -222.0f }, 0.0f },
	};
	for (const auto& data : mansion02List)
	{
		GameObject* mansion02 = Instantiate(mansion02Prefab);
		mansion02->AddComponent<Mansion02>();

		mansion02->transform().scale() = { 1.0f, 1.0f, 1.0f };
		mansion02->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		mansion02->transform().rotation().FromXMVECTOR(q);

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
	struct KaraokeData
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab karaokePrefab{ "model/karaoke.glb" };
	std::vector<KaraokeData> karaokeList =
	{
		//①
		{ { 16.0f, 0.0f, 8.0f },      0.0f },
								
		//②					
		{ { 16.0f, 0.0f, -52.0f },	  0.0f },
		 							
		//③							
		{ { -44.0f, 0.0f, -52.0f },	  0.0f },
		 						
		//④						
		{ { -44.0f, 0.0f, -112.0f },  0.0f },
		 							
		//⑤						
		{ { 16.0f, 0.0f, -112.0f },	  0.0f },
		 							
		//⑥						
		{ { -104.0f, 0.0f, -112.0f }, 0.0f },
		 							
		//⑦						
		{ { -104.0f, 0.0f, -172.0f }, 0.0f },
								
		//⑧						
		{ { -104.0f, 0.0f, -232.0f }, 0.0f },
								
		//⑨							
		{ { -164.0f, 0.0f, -232.0f }, 0.0f },
	};
	for (const auto& data : karaokeList)
	{
		GameObject* karaoke = Instantiate(karaokePrefab);
		karaoke->AddComponent<Karaoke>();

		karaoke->transform().scale() = { 1.0f, 1.0f, 1.0f };
		karaoke->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		karaoke->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 4.0f;
		shapeDesc.SizeY = 3.0f;
		shapeDesc.SizeZ = 4.0f;
		karaoke->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		karaoke->AddComponent<PhysicsBody>(bodyDesc);
	}

	//apartment
	struct ApartmnetData
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab apartmentPrefab{ "model/apartment.glb" };
	std::vector<ApartmnetData> apartmentList =
	{
		//①
		{ { -8.0f, 0.0f,   8.0f },    0.0f },
		{ { 16.0f, 0.0f, -16.0f },	  0.0f },
		 					
		//②						
		{ { -8.0f, 0.0f, -52.0f },	  0.0f },
		{ { 16.0f, 0.0f, -76.0f },	  0.0f },
								
		//③						
		{ { -68.0f, 0.0f, -52.0f },	  0.0f },
		{ { -44.0f, 0.0f, -76.0f },	  0.0f },
								
		//④						
		{ { -68.0f, 0.0f, -112.0f },  0.0f },
		{ { -44.0f, 0.0f, -136.0f },  0.0f },
								
		//⑤						
		{ { -8.0f, 0.0f, -112.0f },	  0.0f },
		{ { 16.0f, 0.0f, -136.0f },	  0.0f },
									
		//⑥						
		{ { -128.0f, 0.0f, -112.0f }, 0.0f },
		{ { -104.0f, 0.0f, -136.0f }, 0.0f },
								
		//⑦						
		{ { -128.0f, 0.0f, -172.0f }, 0.0f },
		{ { -104.0f, 0.0f, -196.0f }, 0.0f },
									
		//⑧						
		{ { -128.0f, 0.0f, -232.0f }, 0.0f },
		{ { -104.0f, 0.0f, -256.0f }, 0.0f },
									
		//⑨						
		{ { -188.0f, 0.0f, -232.0f }, 0.0f },
		{ { -164.0f, 0.0f, -256.0f }, 0.0f },
	};
	for (const auto& data : apartmentList)
	{
		GameObject* apartment = Instantiate(apartmentPrefab);
		apartment->AddComponent<Apartment>();

		apartment->transform().scale() = { 1.0f, 1.0f, 1.0f };
		apartment->transform().position() = data.position;
		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		apartment->transform().rotation().FromXMVECTOR(q);

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
	struct ClockTowerData
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab clocktowerPrefab{ "model/clocktower01.glb" };
	std::vector<ClockTowerData> clocktowerList =
	{
		//①
		{ { -11.0f, 0.0f, -11.0f },   0.0f },
								
		//②							
		{ { -11.0f, 0.0f, -71.0f },	  0.0f },
		 							
		//③							
		{ { -71.0f, 0.0f, -71.0f },	  0.0f },
		 							  
		//④							
		{ { -71.0f, 0.0f, -131.0f },  0.0f },
		 							
		//⑤							
		{ { -11.0f, 0.0f, -131.0f },  0.0f },
		 							
		//⑥							
		{ { -131.0f, 0.0f, -131.0f }, 0.0f },
									
		//⑦							
		{ { -131.0f, 0.0f, -191.0f }, 0.0f },
									 
		//⑧							
		{ { -131.0f, 0.0f, -251.0f }, 0.0f },
		 							
		//⑨							
		{ { -191.0f, 0.0f, -251.0f }, 0.0f },
	};
	for (const auto& data : clocktowerList)
	{
		GameObject* clocktower = Instantiate(clocktowerPrefab);
		clocktower->AddComponent<Clocktower>();

		clocktower->transform().scale() = { 0.5f, 0.5f, 0.5f };
		clocktower->transform().position() = data.position;
		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		clocktower->transform().rotation().FromXMVECTOR(q);

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
	struct WacdonaldData
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab wacdonaldPrefab{ "model/wacdonald.glb" };
	std::vector<WacdonaldData> wacdonaldList =
	{
		//①
		{ { 14.0f, 0.0f, -6.0f },	  180.0f },
		 							 
		//②						  
		{ { 14.0f, 0.0f, -66.0f },	  180.0f },
									
		//③						
		{ { -46.0f, 0.0f, -66.0f },	  180.0f },
		 							
		//④					
		{ { -46.0f, 0.0f, -126.0f },  180.0f },
		 							
		//⑤						
		{ { 14.0f, 0.0f, -126.0f },	  180.0f },
		 							
		//⑥						 
		{ { -106.0f, 0.0f, -126.0f }, 180.0f },
		 							
		//⑦						  
		{ { -106.0f, 0.0f, -186.0f }, 180.0f },
		 							
		//⑧						
		{ { -106.0f, 0.0f, -246.0f }, 180.0f },
									
		//⑨						
		{ { -166.0f, 0.0f, -246.0f }, 180.0f },
	};
	for (const auto& data : wacdonaldList)
	{
		GameObject* wacdonald = Instantiate(wacdonaldPrefab);
		wacdonald->AddComponent<Wacdonald>();

		wacdonald->transform().scale() = { 1.0f, 1.0f, 1.0f };
		wacdonald->transform().position() = data.position;
		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		wacdonald->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 7.0f;
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
		//①
		{ 0.0f, 0.0f, 0.0f },

		//②
		{ 0.0f, 0.0f, -60.0f },

		//③
		{ -60.0f, 0.0f, -60.0f },

		//④
		{ -60.0f, 0.0f, -120.0f },

		//⑤
		{ 0.0f, 0.0f, -120.0f },

		//⑥
		{ -120.0f, 0.0f, -120.0f },

		//⑦
		{ -120.0f, 0.0f, -180.0f },

		//⑧
		{ -120.0f, 0.0f, -240.0f},

		//⑨
		{ -180.0f, 0.0f, -240.0f },
	};
	for (const auto& pos : powerplantPositions)
	{
		// 親オブジェクト（PowerPlantコンポーネント付き）
		GameObject* plantObj = CreateGameObject();
		plantObj->SetName("PowerPlant");
		plantObj->SetTag("PowerPlant");
		plantObj->transform().position() = pos;
		auto* plant = plantObj->AddComponent<PowerPlant>(10.0f);

		// 当たり判定は親オブジェクトに設定
		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 5.0f;
		shapeDesc.SizeY = 6.0f;
		shapeDesc.SizeZ = 6.0f;
		plantObj->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		plantObj->AddComponent<PhysicsBody>(bodyDesc);

		// 復旧済みモデル（Start()で非表示にする）
		GameObject* restored = Instantiate(powerplantPrefab);
		restored->transform().position() = pos;

		// 故障モデル（初期は表示）
		GameObject* broken = Instantiate(powerplant_brokenPrefab);
		broken->transform().position() = pos;

		// PowerPlantにモデル参照を設定
		plant->restoredModel = restored;
		plant->brokenModel = broken;
	}

	//dentyuu
	ModelPrefab dentyuuPrefab{ "model/dentyuu.glb" };
	std::vector<Vector3> dentyuuPositions =
	{
		//①
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

		//②
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

		//③
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

		//④
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

		//⑤
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

		//⑥
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

		//⑦
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

		//⑧
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

		//⑨
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
	struct SpedenData
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab spedentyuPrefab{ "model/special_dentyu.glb" };
	std::vector<SpedenData> spedentyuList =
	{
		//①→②
		{ { 0.0f, 0.0f, -18.0f},       0.0f },
		
		//②→①
		{ { 0.0f, 0.0f, -42.0f },      0.0f },

		//②→③						.
		{ { -18.0f, 0.0f, -60.0f },	   0.0f },
									
		//③→②					
		{ { -42.0f, 0.0f, -60.0f },	   0.0f },
									 
		//③→④						
		{ { -60.0f, 0.0f, -78.0f },	   0.0f },
									
		//④→③					
		{ { -60.0f, 0.0f, -102.0f },   0.0f },
									
		//④→⑤					
		{ { -42.0f, 0.0f, -120.0f },   0.0f },
									
		//⑤→④					
		{ { -18.0f, 0.0f, -120.0f },   0.0f },
									  
		//④→⑥						
		{ { -102.0f, 0.0f, -120.0f },  0.0f },
								
		//⑥→④						
		{ { -78.0f, 0.0f, -120.0f },   0.0f },
									
		//⑥→⑦					
		{ { -120.0f, 0.0f, -138.0f },  0.0f },
								
		//⑦→⑥					
		{ { -120.0f, 0.0f, -162.0f },  0.0f },
									
		//⑦→⑧						
		{ { -120.0f, 0.0f, -198.0f },  0.0f },
									 
		//⑧→⑦					
		{ { -120.0f, 0.0f, -222.0f },  0.0f },
		 						
		//⑧→⑨					
		{ { -162.0f, 0.0f, -240.0f },  0.0f },
		 							
		//⑨→⑧						
		{ { -138.0f, 0.0f, -240.0f },  0.0f },


	};

	// SpecialPole
	std::vector<PoleID> specialPoleIDs;
	specialPoleIDs.reserve(spedentyuList.size());

	for (const auto& data : spedentyuList)
	{
		GameObject* spedentyuu = Instantiate(spedentyuPrefab);

		spedentyuu->AddComponent<SpecialDentyuu>();
		auto* pole = spedentyuu->AddComponent<Pole>();

		specialPoleIDs.push_back(poleManager->RegisterPole(pole));

		spedentyuu->transform().scale() = { 0.3f, 0.6f, 0.3f };
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

	//Special_Densen
	struct DensenData
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab spedensenPrefab{ "model/special_densen.glb" };
	std::vector<DensenData> spedensenList =
	{
		// ①→②
		{ { 0.0f, 10.0f, -18.0f },  0.0f },

		// ②→③
		{ { -18.0f, 10.0f, -60.0f },  90.0f },

		// ③→④
		{ { -60.0f, 10.0f, -78.0f },  0.0f },

		// ④→⑤
		{ { -18.0f, 10.0f, -120.0f },  90.0f },

		// ④→⑥
		{ { -78.0f, 10.0f, -120.0f },  90.0f },

		// ⑥→⑦
		{ { -120.0f, 10.0f, -138.0f },  0.0f },

		// ⑦→⑧
		{ { -120.0f, 10.0f, -198.0f },  0.0f },

		// ⑧→⑨
		{ { -138.0f, 10.0f, -240.0f },  90.0f },
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

		GameObject* spedensen = Instantiate(spedensenPrefab);
		const auto& data = spedensenList.at(index);
		GameObject* spedentyuu = Instantiate(spedensenPrefab);
		spedentyuu->AddComponent<SpecialDensen>();
		if (index < specialLinePolePairs.size() && specialPoleIDs.size() > specialLinePolePairs.at(index).second)
		{
			auto [startIndex, endIndex] = specialLinePolePairs.at(index);
			auto* line = spedentyuu->AddComponent<PowerLine>(specialPoleIDs.at(startIndex), specialPoleIDs.at(endIndex));
			poleManager->RegisterPowerLine(line);
		}


		spedensen->transform().scale() = { 0.2f,0.2f,0.18f };
		spedensen->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		spedensen->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 1.0f;
		shapeDesc.SizeY = 1.0f;
		shapeDesc.SizeZ = 1.0f;
		spedensen->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		spedensen->AddComponent<PhysicsBody>(bodyDesc);
	}

	//Building01
	struct Building01Data
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab building01Prefab{ "model/building01.glb" };
	std::vector<Building01Data> building01List =
	{
	};
	for (const auto& data : building01List)
	{
		GameObject* building01 = Instantiate(building01Prefab);

		building01->transform().scale() = { 30.0f, 30.0f, 30.0f };
		building01->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		building01->transform().rotation().FromXMVECTOR(q);


		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 0.15f;
		shapeDesc.SizeY = 0.1f;
		shapeDesc.SizeZ = 0.1f;
		building01->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		building01->AddComponent<PhysicsBody>(bodyDesc);
	}

	//Building02
	struct Building02Data
	{
		Vector3 position;
		float yRotation; // Y軸回転（度）
	};
	ModelPrefab building02Prefab{ "model/building02.glb" };
	std::vector<Building02Data> building02List =
	{
	};
	for (const auto& data : building02List)
	{
		GameObject* building02 = Instantiate(building02Prefab);

		building02->transform().scale() = { 60.0f, 60.0f, 60.0f };
		building02->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		building02->transform().rotation().FromXMVECTOR(q);


		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 0.1f;
		shapeDesc.SizeY = 10.0f;
		shapeDesc.SizeZ = 0.1f;
		building02->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		building02->AddComponent<PhysicsBody>(bodyDesc);
	}

	// 全発電所の復旧チェック用GameObjectを作成
	GameObject* checker = CreateGameObject();
	checker->AddComponent<GameClearChecker>();


	// UI表示
	GameObject* uiRoot = CreateGameObject();
	uiRoot->SetName("UIRoot");

	// 画面中央に 500x500 の UI（y は負にしない）
	UI::CreateUI(
		uiRoot,
		L"texture/BRIGHTOUT_UI_TAIMA-kl.png",
		Vector3(700.0f, -100.0f, 0.0f),   // ピクセル座標 (x, y)
		Vector3(500.0f, 500.0f, 1.0f),   // 幅=500px, 高さ=500px
		"UiVS.cso",
		"UiPS.cso"
	);

	// タイマー（位置はピクセル、スケールは TimerUI が内部で使うので 1,1,1 に）
	GameObject* timerRoot = uiRoot->scenePtr()->CreateGameObject();
	timerRoot->SetName("TimerRoot");
	timerRoot->transform().position() = Vector3(925.0f, 100.0f, 0.0f); 
	timerRoot->transform().scale() = Vector3(1.0f, 1.0f, 1.0f);
	timerRoot->AddComponent<TimerUI>();

	// HP バー（HPBarUI の既定 scale は 400x400 を想定）
	GameObject* hpRoot = uiRoot->scenePtr()->CreateGameObject();
	hpRoot->SetName("HPBarRoot");
	hpRoot->transform().position() = Vector3(50.0f, 800.0f, 0.0f); // 画面の下寄せ例
	hpRoot->transform().scale() = Vector3(800.0f, 800.0f, 1.0f); 

	auto* hpUi = hpRoot->AddComponent<HPBarUI>();
	hpUi->backgroundTexture = L"texture/BRIGHTOUT_battery_kara.png";
	hpUi->fillTexture = L"texture/BRIGHTOUT_battery_ge-ji.png";
	hpUi->backgroundPosition = hpRoot->transform().position();
	hpUi->fillPosition = Vector3(hpRoot->transform().position().x + 10.0f, hpRoot->transform().position().y, 0.0f);
	hpUi->backgroundScale = Vector3(400.0f, 400.0f, 1.0f);
	hpUi->fillScale = Vector3(400.0f, 400.0f, 1.0f);
	hpUi->vsPath = "UiVS.cso";
	hpUi->psPath = "UiPS.cso";


	// 変身UI
	GameObject* transAvailable = UI::CreateUI(
		uiRoot,
		L"texture/trans02.png",
		Vector3(1100.0f, 450.0f, 0.0f),   // ピクセル座標
		Vector3(1200.0f, 900.0f, 1.0f),   // スケール
		"UiVS.cso",
		"UiPS.cso"
	);
	if (transAvailable) transAvailable->SetName("TransAvailableUI");

	GameObject* transActive = UI::CreateUI(
		uiRoot,
		L"texture/transcd01.png",
		Vector3(1100.0f, 450.0f, 0.0f),
		Vector3(1200.0f, 900.0f, 1.0f),
		"UiVS.cso",
		"UiPS.cso"
	);
	if (transActive) transActive->SetName("TransActiveUI");

	if (transActive) transActive->SetActive(false);

	// MorphUI コントローラ
	GameObject* morphController = uiRoot->scenePtr()->CreateGameObject();
	morphController->SetName("MorphUIController");
	auto* morphUI = morphController->AddComponent<MorphUI>();
	morphUI->uiAvailable = transAvailable;
	morphUI->uiActive = transActive;
	morphUI->cooldownSeconds = 3.0f;

	// フェードイン演出
	GameObject* fadeObj = CreateGameObject();
	fadeObj->SetName("FadeController");
	auto* fadeCtrl = fadeObj->AddComponent<FadeController>();
	fadeCtrl->StartFadeIn(2.0f);
}

void Namioka::Finalize()
{
	// 音声解放
	StopAudio(g_GameBgm);
	UnloadAudio(g_GameBgm);
	StopAudio(g_GameBgm2);
	UnloadAudio(g_GameBgm2);
}