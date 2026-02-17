#include "Namioka.h"
#include "PlayerPrefab.h"
#include "GameObject.h"
#include "DebugCamera.h"
#include "Texture.h"

using namespace DirectX;

void Namioka::Initialize()
{
	g_Bgm = LoadAudio("asset/bgm.wav");

	//Player
	PlayerPrefab playerPrefab{};
	Instantiate(playerPrefab)->transform().position() += Vector3(0.0f, 0.0f, 10.0f);

	//Road
	ModelPrefab roadPrefab{ "model/intersection.glb" };
	std::vector<Vector3> roadPositions =
	{
		{    0.0f, 0.0f,    0.0f},   //á@
		{    0.0f, 0.0f,  -60.0f},   //áA
		{  -60.0f, 0.0f,  -60.0f},   //áB
		{  -60.0f, 0.0f, -120.0f},   //áC
		{    0.0f, 0.0f, -120.0f},   //áD
		{ -120.0f, 0.0f, -120.0f},   //áE
		{ -120.0f, 0.0f, -180.0f},   //áF
		{ -120.0f, 0.0f, -240.0f},   //áG
		{ -180.0f, 0.0f, -240.0f},   //áH
	};
	for (const auto& pos : roadPositions)
	{
		GameObject* road = Instantiate(roadPrefab);

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
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab housePrefab{ "model/house.glb" };
	std::vector<HouseData> houseList =
	{
		//á@
		{ {  -6.0f, 0.0f,  -6.0f },	 0.0f },
		{ { -18.0f, 0.0f,  -6.0f },	 0.0f },
		{ {  -6.0f, 0.0f, -18.0f },	 0.0f },
		{ { -18.0f, 0.0f, -18.0f },	 0.0f },

		//áA
		{ {  -6.0f, 0.0f, -66.0f },  0.0f },
		{ { -18.0f, 0.0f, -66.0f },  0.0f },
		{ {  -6.0f, 0.0f, -78.0f },  0.0f },
		{ { -18.0f, 0.0f, -78.0f },  0.0f },

		//áB
		{ { -66.0f, 0.0f, -66.0f },	 0.0f },
		{ { -78.0f, 0.0f, -66.0f },	 0.0f },
		{ { -66.0f, 0.0f, -78.0f },	 0.0f },
		{ { -78.0f, 0.0f, -78.0f },	 0.0f },

		//áC
		{ { -66.0f, 0.0f, -126.0f }, 0.0f },
		{ { -78.0f, 0.0f, -126.0f }, 0.0f },
		{ { -66.0f, 0.0f, -138.0f }, 0.0f },
		{ { -78.0f, 0.0f, -138.0f }, 0.0f },

		//áD
		{ {  -6.0f, 0.0f, -126.0f }, 0.0f },
		{ { -18.0f, 0.0f, -126.0f }, 0.0f },
		{ {  -6.0f, 0.0f, -138.0f }, 0.0f },
		{ { -18.0f, 0.0f, -138.0f }, 0.0f },

		//áE
		{ { -126.0f, 0.0f, -126.0f }, 0.0f },
		{ { -138.0f, 0.0f, -126.0f }, 0.0f },
		{ { -126.0f, 0.0f, -138.0f }, 0.0f },
		{ { -138.0f, 0.0f, -138.0f }, 0.0f },

		//áF
		{ { -126.0f, 0.0f, -186.0f },  0.0f },
		{ { -138.0f, 0.0f, -186.0f },  0.0f },
		{ { -126.0f, 0.0f, -198.0f },  0.0f },
		{ { -138.0f, 0.0f, -198.0f },  0.0f },

		//áG
		{ { -126.0f, 0.0f, -246.0f },  0.0f },
		{ { -138.0f, 0.0f, -246.0f },  0.0f },
		{ { -126.0f, 0.0f, -258.0f },  0.0f },
		{ { -138.0f, 0.0f, -258.0f },  0.0f },

		//áH
		{ { -186.0f, 0.0f, -246.0f },  0.0f },
		{ { -198.0f, 0.0f, -246.0f },  0.0f },
		{ { -186.0f, 0.0f, -258.0f },  0.0f },
		{ { -198.0f, 0.0f, -258.0f },  0.0f },
	};
	for (const auto& data : houseList)
	{
		GameObject* house = Instantiate(housePrefab);

		house->transform().scale() = { 1.0f, 1.0f, 1.0f };
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
	struct ConveniData
	{
		Vector3 position;
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab conveniencePrefab{ "model/convenience.glb" };
	std::vector<ConveniData> conveniList =
	{
		//á@
		{{ 6.0f, 0.0f,   6.0f },      0.0f },
		{{ 6.0f, 0.0f, -16.0f },	  0.0f },
									 
		//áA						    
		{{ 6.0f, 0.0f, -54.0f },	  0.0f },
		{{ 6.0f, 0.0f, -76.0f },	  0.0f },
									 
		//áB						  
		{{ -54.0f, 0.0f, -54.0f },	  0.0f },
		{{ -54.0f, 0.0f, -76.0f },	  0.0f },
									 
		//áC						   
		{{ -54.0f, 0.0f, -114.0f },	  0.0f },
		{{ -54.0f, 0.0f, -136.0f },	  0.0f },
									 
		//áD						   
		{{ 6.0f, 0.0f, -114.0f },	  0.0f },
		{{ 6.0f, 0.0f, -136.0f },	  0.0f },
									  
		//áE						  
		{{ -114.0f, 0.0f, -114.0f },  0.0f },
		{{ -114.0f, 0.0f, -136.0f },  0.0f },
									
		//áF						    
		{{ -114.0f, 0.0f, -174.0f },  0.0f },
		{{ -114.0f, 0.0f, -196.0f },  0.0f },
									 
		//áG						    
		{{ -114.0f, 0.0f, -234.0f },  0.0f },
		{{ -114.0f, 0.0f, -256.0f },  0.0f },
									
		//áH						    
		{{ -174.0f, 0.0f, -234.0f },  0.0f },
		{{ -174.0f, 0.0f, -256.0f },  0.0f },
	};
	for (const auto& data : conveniList)
	{
		GameObject* convenience = Instantiate(conveniencePrefab);

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
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab mansion01Prefab{ "model/mansion01.glb" };
	std::vector<Mansion01Data> mansion01List =
	{
		//á@
		{{  -8.0f, 0.0f, 18.0f },    0.0f },
		{{ -16.0f, 0.0f,  8.0f },	 0.0f },
		{{   6.0f, 0.0f, -6.0f },	 0.0f },
									
		//áA						
		{{  -8.0f, 0.0f, -42.0f },	 0.0f },
		{{ -16.0f, 0.0f, -52.0f },	 0.0f },
		{{   6.0f, 0.0f, -66.0f },	 0.0f },
									
		//áB						
		{{ -68.0f, 0.0f, -42.0f },	 0.0f },
		{{ -76.0f, 0.0f, -52.0f },	 0.0f },
		{{ -54.0f, 0.0f, -66.0f },	 0.0f },
								
		//áC						
		{{ -68.0f, 0.0f, -102.0f },	 0.0f },
		{{ -76.0f, 0.0f, -112.0f },	 0.0f },
		{{ -54.0f, 0.0f, -126.0f },	 0.0f },
									
		//áD						
		{{  -8.0f, 0.0f, -102.0f },	 0.0f },
		{{ -16.0f, 0.0f, -112.0f },	 0.0f },
		{{   6.0f, 0.0f, -126.0f },	 0.0f },
									
		//áE							
		{{ -128.0f, 0.0f, -102.0f }, 0.0f },
		{{ -136.0f, 0.0f, -112.0f }, 0.0f },
		{{ -114.0f, 0.0f, -126.0f }, 0.0f },
									
		//áF						
		{{ -128.0f, 0.0f, -162.0f }, 0.0f },
		{{ -136.0f, 0.0f, -172.0f }, 0.0f },
		{{ -114.0f, 0.0f, -186.0f }, 0.0f },
									
		//áG						
		{{ -128.0f, 0.0f, -222.0f }, 0.0f },
		{{ -136.0f, 0.0f, -232.0f }, 0.0f },
		{{ -114.0f, 0.0f, -246.0f }, 0.0f },
									
		//áH						
		{{ -188.0f, 0.0f, -222.0f }, 0.0f },
		{{ -196.0f, 0.0f, -232.0f }, 0.0f },
		{{ -174.0f, 0.0f, -246.0f }, 0.0f },
	};
	for (const auto& data : mansion01List)
	{
		GameObject* mansion01 = Instantiate(mansion01Prefab);

		mansion01->transform().scale() = { 1.0f, 1.0f, 1.0f };
		mansion01->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		mansion01->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 4.0f;
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
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab mansion02Prefab{ "model/mansion02.glb" };
	std::vector<Mansion02Data> mansion02List =
	{
		//á@
		{{ 8.0f, 0.0f, 18.0f },      0.0f },
									
		//áA						   
		{{ 8.0f, 0.0f, -42.0f },	 0.0f },
									
		//áB						  
		{{ -52.0f, 0.0f, -42.0f },	 0.0f },
									
		//áC						   
		{{ -52.0f, 0.0f, -102.0f },	 0.0f },
									
		//áD						   
		{{ 8.0f, 0.0f, -102.0f },	 0.0f },
									 
		//áE						   
		{{ -112.0f, 0.0f, -102.0f }, 0.0f },
									 
		//áF						  
		{{ -112.0f, 0.0f, -162.0f }, 0.0f },
									
		//áG						 
		{{ -112.0f, 0.0f, -222.0f }, 0.0f },
									
		//áH						   
		{{ -172.0f, 0.0f, -222.0f }, 0.0f },
	};
	for (const auto& data : mansion02List)
	{
		GameObject* mansion02 = Instantiate(mansion02Prefab);

		mansion02->transform().scale() = { 1.0f, 1.0f, 0.8f };
		mansion02->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		mansion02->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 8.0f;
		shapeDesc.SizeY = 5.0f;
		shapeDesc.SizeZ = 8.0f;
		mansion02->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		mansion02->AddComponent<PhysicsBody>(bodyDesc);
	}

	//karaoke
	struct KaraokeData
	{
		Vector3 position;
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab karaokePrefab{ "model/karaoke.glb" };
	std::vector<KaraokeData> karaokeList =
	{
		//á@
		{{ 16.0f, 0.0f, 8.0f },      0.0f },
									 
		//áA						   
		{{ 16.0f, 0.0f, -52.0f },	 0.0f },
									 
		//áB						   
		{{ -44.0f, 0.0f, -52.0f },	 0.0f },
									
		//áC						   
		{{ -44.0f, 0.0f, -112.0f },	 0.0f },
									 
		//áD						   
		{{ 16.0f, 0.0f, -112.0f },	 0.0f },
									 
		//áE						   
		{{ -104.0f, 0.0f, -112.0f }, 0.0f },
									 
		//áF						  
		{{ -104.0f, 0.0f, -172.0f }, 0.0f },
									 
		//áG						   
		{{ -104.0f, 0.0f, -232.0f }, 0.0f },
									
		//áH						    
		{{ -164.0f, 0.0f, -232.0f }, 0.0f },
	};
	for (const auto& data : karaokeList)
	{
		GameObject* karaoke = Instantiate(karaokePrefab);

		karaoke->transform().scale() = { 1.0f, 1.0f, 1.0f };
		karaoke->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		karaoke->transform().rotation().FromXMVECTOR(q);

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
	struct ApartmentData
	{
		Vector3 position;
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab apartmentPrefab{ "model/apartment.glb" };
	std::vector<ApartmentData> apartmentList =
	{
		//á@
		{ { -8.0f, 0.0f,   8.0f },    0.0f },
		{ { 16.0f, 0.0f, -16.0f },	  0.0f },
									  
		//áA							
		{ { -8.0f, 0.0f, -52.0f },	  0.0f },
		{ { 16.0f, 0.0f, -76.0f },	  0.0f },
		 							
		//áB						
		{ { -68.0f, 0.0f, -52.0f },	  0.0f },
		{ { -44.0f, 0.0f, -76.0f },	  0.0f },
		 							  
		//áC							
		{ { -68.0f, 0.0f, -112.0f },  0.0f },
		{ { -44.0f, 0.0f, -136.0f },  0.0f },
		 							  
		//áD							
		{ { -8.0f, 0.0f, -112.0f },	  0.0f },
		{ { 16.0f, 0.0f, -136.0f },	  0.0f },
									 
		//áE							
		{ { -128.0f, 0.0f, -112.0f }, 0.0f },
		{ { -104.0f, 0.0f, -136.0f }, 0.0f },
		 							 
		//áF							
		{ { -128.0f, 0.0f, -172.0f }, 0.0f },
		{ { -104.0f, 0.0f, -196.0f }, 0.0f },
		 							  
		//áG							
		{ { -128.0f, 0.0f, -232.0f }, 0.0f },
		{ { -104.0f, 0.0f, -256.0f }, 0.0f },
		 							  
		//áH							
		{ { -188.0f, 0.0f, -232.0f }, 0.0f },
		{ { -164.0f, 0.0f, -256.0f }, 0.0f },
	};
	for (const auto& data : apartmentList)
	{
		GameObject* apartment = Instantiate(apartmentPrefab);

		apartment->transform().scale() = { 1.0f, 1.0f, 1.0f };
		apartment->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		apartment->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 10.0f;
		shapeDesc.SizeY = 16.0f;
		shapeDesc.SizeZ = 7.0f;
		apartment->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		apartment->AddComponent<PhysicsBody>(bodyDesc);
	}


	//clocktower
	struct ClockTowerData
	{
		Vector3 position;
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab clocktowerPrefab{ "model/clocktower01.glb" };
	std::vector<ClockTowerData> clocktowerList =
	{
		//á@
		{ { -11.0f, 0.0f, -11.0f },   0.0f },
		 							  
		//áA							
		{ { -11.0f, 0.0f, -71.0f },	  0.0f },
									  
		//áB						
		{ { -71.0f, 0.0f, -71.0f },	  0.0f },
		 							 
		//áC							
		{ { -71.0f, 0.0f, -131.0f },  0.0f },
									 
		//áD							
		{ { -11.0f, 0.0f, -131.0f },  0.0f },
									 
		//áE							
		{ { -131.0f, 0.0f, -131.0f }, 0.0f },
		 							  
		//áF							
		{ { -131.0f, 0.0f, -191.0f }, 0.0f },
									 
		//áG							
		{ { -131.0f, 0.0f, -251.0f }, 0.0f },
									 
		//áH							
		{ { -191.0f, 0.0f, -251.0f }, 0.0f },
	};
	for (const auto& data : clocktowerList)
	{
		GameObject* clocktower = Instantiate(clocktowerPrefab);

		clocktower->transform().scale() = { 0.5f, 0.5f, 0.5f };
		clocktower->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		clocktower->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 8.0f;
		shapeDesc.SizeY = 4.0f;
		shapeDesc.SizeZ = 7.0f;
		clocktower->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		clocktower->AddComponent<PhysicsBody>(bodyDesc);
	}

	//wacdonald
	struct WacdonaldData
	{
		Vector3 position;
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab wacdonaldPrefab{ "model/wacdonald.glb" };
	std::vector<WacdonaldData> wacdonaldList =
	{
		//á@
		{{ 14.0f, 0.0f, -6.0f },     180.0f },
								
		//áA						
		{{ 14.0f, 0.0f, -66.0f },	 180.0f },
									
		//áB					
		{{ -46.0f, 0.0f, -66.0f },	 180.0f },
									
		//áC						 
		{{ -46.0f, 0.0f, -126.0f },	 180.0f },
									
		//áD						 
		{{ 14.0f, 0.0f, -126.0f },	 180.0f },
									
		//áE						  
		{{ -106.0f, 0.0f, -126.0f }, 180.0f },
									
		//áF						  
		{{ -106.0f, 0.0f, -186.0f }, 180.0f },
									
		//áG						  
		{{ -106.0f, 0.0f, -246.0f }, 180.0f },
									
		//áH						  
		{{ -166.0f, 0.0f, -246.0f }, 180.0f },
	};
	for (const auto& data : wacdonaldList)
	{
		GameObject* wacdonald = Instantiate(wacdonaldPrefab);

		wacdonald->transform().scale() = { 1.0f, 1.0f, 1.0f };
		wacdonald->transform().position() = data.position;
		
		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		wacdonald->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 8.0f;
		shapeDesc.SizeY = 4.0f;
		shapeDesc.SizeZ = 5.0f;
		wacdonald->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		wacdonald->AddComponent<PhysicsBody>(bodyDesc);
	}

	//powerplant
	struct PowerplantData
	{
		Vector3 position;
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab powerplantPrefab{ "model/powerplant.glb" };
	std::vector<PowerplantData> powerplantList =
	{
		//á@
		{{ 0.0f, 0.0f, 0.0f },       0.0f },
									
		//áA						 
		{{ 0.0f, 0.0f, -60.0f },	 0.0f },
									
		//áB						
		{{ -60.0f, 0.0f, -60.0f },	 0.0f },
									
		//áC						
		{{ -60.0f, 0.0f, -120.0f },	 0.0f },
									
		//áD						
		{{ 0.0f, 0.0f, -120.0f },	 0.0f },
									
		//áE					
		{{ -120.0f, 0.0f, -120.0f }, 0.0f },
									
		//áF						
		{{ -120.0f, 0.0f, -180.0f }, 0.0f },
									
		//áG						  
		{{ -120.0f, 0.0f, -240.0f},	 0.0f },
									
		//áH						   
		{{ -180.0f, 0.0f, -240.0f }, 0.0f },
	};
	for (const auto& data : powerplantList)
	{
		GameObject* powerplant = Instantiate(powerplantPrefab);

		powerplant->transform().scale() = { 1.0f, 1.0f, 1.0f };
		powerplant->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		powerplant->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 6.0f;
		shapeDesc.SizeY = 6.0f;
		shapeDesc.SizeZ = 6.0f;
		powerplant->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		powerplant->AddComponent<PhysicsBody>(bodyDesc);
	}

	//powerplant_broken
	struct Powerplant_BrokenData
	{
		Vector3 position;
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab powerplant_brokenPrefab{ "model/powerplant_broken.glb" };
	std::vector<Powerplant_BrokenData> powerplant_brokenList =
	{
		//á@
		{{ 0.0f, 0.0f, 0.0f },       0.0f },

		//áA						 
		{{ 0.0f, 0.0f, -60.0f },	 0.0f },

		//áB						
		{{ -60.0f, 0.0f, -60.0f },	 0.0f },

		//áC						
		{{ -60.0f, 0.0f, -120.0f },	 0.0f },

		//áD						
		{{ 0.0f, 0.0f, -120.0f },	 0.0f },

		//áE					
		{{ -120.0f, 0.0f, -120.0f }, 0.0f },

		//áF						
		{{ -120.0f, 0.0f, -180.0f }, 0.0f },

		//áG						  
		{{ -120.0f, 0.0f, -240.0f},	 0.0f },

		//áH						   
		{{ -180.0f, 0.0f, -240.0f }, 0.0f },
	};
	for (const auto& data : powerplant_brokenList)
	{
		GameObject* powerplant_broken = Instantiate(powerplant_brokenPrefab);

		powerplant_broken->transform().scale() = { 1.0f, 1.0f, 1.0f };
		powerplant_broken->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		powerplant_broken->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 6.0f;
		shapeDesc.SizeY = 6.0f;
		shapeDesc.SizeZ = 6.0f;
		powerplant_broken->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		powerplant_broken->AddComponent<PhysicsBody>(bodyDesc);
	}

	//dentyuu
	struct DentyuData
	{
		Vector3 position;
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab dentyuuPrefab{ "model/dentyuu.glb" };
	std::vector<DentyuData> dentyuuList =
	{
		//á@
		{{   3.0f, 2.0f,  10.0f }, 0.0f},
		{{   3.0f, 2.0f,  20.0f }, 0.0f},
		{{  -3.0f, 2.0f,  10.0f }, 0.0f},
		{{  -3.0f, 2.0f,  20.0f }, 0.0f},
		{{   3.0f, 2.0f, -10.0f }, 0.0f},
		{{   3.0f, 2.0f, -20.0f }, 0.0f},
		{{  -3.0f, 2.0f, -10.0f }, 0.0f},
		{{  -3.0f, 2.0f, -20.0f }, 0.0f},
		{{   9.0f, 2.0f,   3.0f }, 0.0f},
		{{  18.0f, 2.0f,   3.0f }, 0.0f},
		{{   9.0f, 2.0f,  -3.0f }, 0.0f},
		{{  18.0f, 2.0f,  -3.0f }, 0.0f},
		{{  -9.0f, 2.0f,   3.0f }, 0.0f},
		{{ -18.0f, 2.0f,   3.0f }, 0.0f},
		{{  -9.0f, 2.0f,  -3.0f }, 0.0f},
		{{ -18.0f, 2.0f,  -3.0f }, 0.0f},

		//áA
		{{   3.0f, 2.0f, -50.0f }, 0.0f},
		{{   3.0f, 2.0f, -40.0f }, 0.0f},
		{{  -3.0f, 2.0f, -50.0f }, 0.0f},
		{{  -3.0f, 2.0f, -40.0f }, 0.0f},
		{{   3.0f, 2.0f, -70.0f }, 0.0f},
		{{   3.0f, 2.0f, -80.0f }, 0.0f},
		{{  -3.0f, 2.0f, -70.0f }, 0.0f},
		{{  -3.0f, 2.0f, -80.0f }, 0.0f},
		{{   9.0f, 2.0f, -57.0f }, 0.0f},
		{{  18.0f, 2.0f, -57.0f }, 0.0f},
		{{   9.0f, 2.0f, -63.0f }, 0.0f},
		{{  18.0f, 2.0f, -63.0f }, 0.0f},
		{{  -9.0f, 2.0f, -57.0f }, 0.0f},
		{{ -18.0f, 2.0f, -57.0f }, 0.0f},
		{{  -9.0f, 2.0f, -63.0f }, 0.0f},
		{{ -18.0f, 2.0f, -63.0f }, 0.0f},

		//áB
		{{ -57.0f, 2.0f, -50.0f }, 0.0f},
		{{ -57.0f, 2.0f, -40.0f }, 0.0f},
		{{ -63.0f, 2.0f, -50.0f }, 0.0f},
		{{ -63.0f, 2.0f, -40.0f }, 0.0f},
		{{ -57.0f, 2.0f, -70.0f }, 0.0f},
		{{ -57.0f, 2.0f, -80.0f }, 0.0f},
		{{ -63.0f, 2.0f, -70.0f }, 0.0f},
		{{ -63.0f, 2.0f, -80.0f }, 0.0f},
		{{ -51.0f, 2.0f, -57.0f }, 0.0f},
		{{ -42.0f, 2.0f, -57.0f }, 0.0f},
		{{ -51.0f, 2.0f, -63.0f }, 0.0f},
		{{ -42.0f, 2.0f, -63.0f }, 0.0f},
		{{ -69.0f, 2.0f, -57.0f }, 0.0f},
		{{ -78.0f, 2.0f, -57.0f }, 0.0f},
		{{ -69.0f, 2.0f, -63.0f }, 0.0f},
		{{ -78.0f, 2.0f, -63.0f }, 0.0f},

		//áC
		{{ -57.0f, 2.0f, -110.0f },0.0f},
		{{ -57.0f, 2.0f, -100.0f },0.0f},
		{{ -63.0f, 2.0f, -110.0f },0.0f},
		{{ -63.0f, 2.0f, -100.0f },0.0f},
		{{ -57.0f, 2.0f, -130.0f },0.0f},
		{{ -57.0f, 2.0f, -140.0f },0.0f},
		{{ -63.0f, 2.0f, -130.0f },0.0f},
		{{ -63.0f, 2.0f, -140.0f },0.0f},
		{{ -51.0f, 2.0f, -117.0f },0.0f},
		{{ -42.0f, 2.0f, -117.0f },0.0f},
		{{ -51.0f, 2.0f, -123.0f },0.0f},
		{{ -42.0f, 2.0f, -123.0f },0.0f},
		{{ -69.0f, 2.0f, -117.0f },0.0f},
		{{ -78.0f, 2.0f, -117.0f },0.0f},
		{{ -69.0f, 2.0f, -123.0f },0.0f},
		{{ -78.0f, 2.0f, -123.0f },0.0f},

		//áD
		{{   3.0f, 2.0f, -110.0f },0.0f},
		{{   3.0f, 2.0f, -100.0f },0.0f},
		{{  -3.0f, 2.0f, -110.0f },0.0f},
		{{  -3.0f, 2.0f, -100.0f },0.0f},
		{{   3.0f, 2.0f, -130.0f },0.0f},
		{{   3.0f, 2.0f, -140.0f },0.0f},
		{{  -3.0f, 2.0f, -130.0f },0.0f},
		{{  -3.0f, 2.0f, -140.0f },0.0f},
		{{   9.0f, 2.0f, -117.0f },0.0f},
		{{  18.0f, 2.0f, -117.0f },0.0f},
		{{   9.0f, 2.0f, -123.0f },0.0f},
		{{  18.0f, 2.0f, -123.0f },0.0f},
		{{  -9.0f, 2.0f, -117.0f },0.0f},
		{{ -18.0f, 2.0f, -117.0f },0.0f},
		{{  -9.0f, 2.0f, -123.0f },0.0f},
		{{ -18.0f, 2.0f, -123.0f },0.0f},

		//áE
		{{ -117.0f, 2.0f, -110.0f },0.0f},
		{{ -117.0f, 2.0f, -100.0f },0.0f},
		{{ -123.0f, 2.0f, -110.0f },0.0f},
		{{ -123.0f, 2.0f, -100.0f },0.0f},
		{{ -117.0f, 2.0f, -130.0f },0.0f},
		{{ -117.0f, 2.0f, -140.0f },0.0f},
		{{ -123.0f, 2.0f, -130.0f },0.0f},
		{{ -123.0f, 2.0f, -140.0f },0.0f},
		{{ -111.0f, 2.0f, -117.0f },0.0f},
		{{ -102.0f, 2.0f, -117.0f },0.0f},
		{{ -111.0f, 2.0f, -123.0f },0.0f},
		{{ -102.0f, 2.0f, -123.0f },0.0f},
		{{ -129.0f, 2.0f, -117.0f },0.0f},
		{{ -138.0f, 2.0f, -117.0f },0.0f},
		{{ -129.0f, 2.0f, -123.0f },0.0f},
		{{ -138.0f, 2.0f, -123.0f },0.0f},

		//áF
		{{ -117.0f, 2.0f, -170.0f }, 0.0f},
		{{ -117.0f, 2.0f, -160.0f }, 0.0f},
		{{ -123.0f, 2.0f, -170.0f }, 0.0f},
		{{ -123.0f, 2.0f, -160.0f }, 0.0f},
		{{ -117.0f, 2.0f, -190.0f }, 0.0f},
		{{ -117.0f, 2.0f, -200.0f }, 0.0f},
		{{ -123.0f, 2.0f, -190.0f }, 0.0f},
		{{ -123.0f, 2.0f, -200.0f }, 0.0f},
		{{ -111.0f, 2.0f, -177.0f }, 0.0f},
		{{ -102.0f, 2.0f, -177.0f }, 0.0f},
		{{ -111.0f, 2.0f, -183.0f }, 0.0f},
		{{ -102.0f, 2.0f, -183.0f }, 0.0f},
		{{ -129.0f, 2.0f, -177.0f }, 0.0f},
		{{ -138.0f, 2.0f, -177.0f }, 0.0f},
		{{ -129.0f, 2.0f, -183.0f }, 0.0f},
		{{ -138.0f, 2.0f, -183.0f }, 0.0f},

		//áG
		{{ -117.0f, 2.0f, -230.0f }, 0.0f},
		{{ -117.0f, 2.0f, -220.0f }, 0.0f},
		{{ -123.0f, 2.0f, -230.0f }, 0.0f},
		{{ -123.0f, 2.0f, -220.0f }, 0.0f},
		{{ -117.0f, 2.0f, -250.0f }, 0.0f},
		{{ -117.0f, 2.0f, -260.0f }, 0.0f},
		{{ -123.0f, 2.0f, -250.0f }, 0.0f},
		{{ -123.0f, 2.0f, -260.0f }, 0.0f},
		{{ -111.0f, 2.0f, -237.0f }, 0.0f},
		{{ -102.0f, 2.0f, -237.0f }, 0.0f},
		{{ -111.0f, 2.0f, -243.0f }, 0.0f},
		{{ -102.0f, 2.0f, -243.0f }, 0.0f},
		{{ -129.0f, 2.0f, -237.0f }, 0.0f},
		{{ -138.0f, 2.0f, -237.0f }, 0.0f},
		{{ -129.0f, 2.0f, -243.0f }, 0.0f},
		{{ -138.0f, 2.0f, -243.0f }, 0.0f},

		//áH
		{{ -177.0f, 2.0f, -230.0f }, 0.0f},
		{{ -177.0f, 2.0f, -220.0f }, 0.0f},
		{{ -183.0f, 2.0f, -230.0f }, 0.0f},
		{{ -183.0f, 2.0f, -220.0f }, 0.0f},
		{{ -177.0f, 2.0f, -250.0f }, 0.0f},
		{{ -177.0f, 2.0f, -260.0f }, 0.0f},
		{{ -183.0f, 2.0f, -250.0f }, 0.0f},
		{{ -183.0f, 2.0f, -260.0f }, 0.0f},
		{{ -171.0f, 2.0f, -237.0f }, 0.0f},
		{{ -162.0f, 2.0f, -237.0f }, 0.0f},
		{{ -171.0f, 2.0f, -243.0f }, 0.0f},
		{{ -162.0f, 2.0f, -243.0f }, 0.0f},
		{{ -189.0f, 2.0f, -237.0f }, 0.0f},
		{{ -198.0f, 2.0f, -237.0f }, 0.0f},
		{{ -189.0f, 2.0f, -243.0f }, 0.0f},
		{{ -198.0f, 2.0f, -243.0f }, 0.0f},
	};
	for (const auto& data : dentyuuList)
	{
		GameObject* dentyuu = Instantiate(dentyuuPrefab);

		dentyuu->transform().scale() = { 10.0f, 8.0f, 10.0f };
		dentyuu->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		dentyuu->transform().rotation().FromXMVECTOR(q);

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
	struct SpeDenData
	{
		Vector3 position;
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab spedentyuPrefab{ "model/special_dentyu.glb" };
	std::vector<SpeDenData> spedentyuList =
	{
		//á@Å®áA
		{{ 0.0f, 0.0f, -18.0f},      0.0f },
								
		//áAÅ®á@					
		{{ 0.0f, 0.0f, -42.0f },	 0.0f },
								
		//áAÅ®áB					
		{{ -18.0f, 0.0f, -60.0f },	 0.0f },
									
		//áBÅ®áA					
		{{ -42.0f, 0.0f, -60.0f },	 0.0f },
									
		//áBÅ®áC					 
		{{ -60.0f, 0.0f, -78.0f },	 0.0f },
									
		//áCÅ®áB				
		{{ -60.0f, 0.0f, -102.0f },	 0.0f },
									
		//áCÅ®áD					 
		{{ -42.0f, 0.0f, -120.0f },	 0.0f },
								
		//áDÅ®áC					
		{{ -18.0f, 0.0f, -120.0f },	 0.0f },
									
		//áCÅ®áE					  
		{{ -102.0f, 0.0f, -120.0f }, 0.0f },
								
		//áEÅ®áC					 
		{{ -78.0f, 0.0f, -120.0f },	 0.0f },
									
		//áEÅ®áF					 
		{{ -120.0f, 0.0f, -138.0f }, 0.0f },
									
		//áFÅ®áE					 
		{{ -120.0f, 0.0f, -162.0f }, 0.0f },
								
		//áFÅ®áG					 
		{{ -120.0f, 0.0f, -198.0f }, 0.0f },
									
		//áGÅ®áF					 
		{{ -120.0f, 0.0f, -222.0f }, 0.0f },
									
		//áGÅ®áH					 
		{{ -162.0f, 0.0f, -240.0f }, 0.0f },
									
		//áHÅ®áG
		{{ -138.0f, 0.0f, -240.0f }, 0.0f },


	};
	for (const auto& data : spedentyuList)
	{
		GameObject* spedentyuu = Instantiate(spedentyuPrefab);

		spedentyuu->transform().scale() = { 0.1f, 0.1f, 0.1f };
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
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab spedensenPrefab{ "model/special_densen.glb" };
	std::vector<DensenData> spedensenList =
	{
		// á@Å®áA
		{ { 0.0f, 5.0f, -18.0f },  0.0f },

		// áAÅ®áB
		{ { -18.0f, 5.0f, -60.0f },  90.0f },

		// áBÅ®áC
		{ { -60.0f, 5.0f, -78.0f },  0.0f },

		// áCÅ®áD
		{ { -18.0f, 5.0f, -120.0f },  90.0f },

		// áCÅ®áE
		{ { -78.0f, 5.0f, -120.0f },  90.0f },

		// áEÅ®áF
		{ { -120.0f, 5.0f, -138.0f },  0.0f },

		// áFÅ®áG
		{ { -120.0f, 5.0f, -198.0f },  0.0f },

		// áGÅ®áH
		{ { -138.0f, 5.0f, -240.0f },  90.0f },
	};

	for (const auto& data : spedensenList)
	{
		GameObject* spedensen = Instantiate(spedensenPrefab);

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
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab building01Prefab{ "model/building01.glb" };
	std::vector<Building01Data> building01List =
	{
	};

	for (const auto& data : building01List)
	{
		GameObject* building01 = Instantiate(building01Prefab);

		building01->transform().scale() = { 30.0f,30.0f,30.0f };
		building01->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		building01->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 0.2f;
		shapeDesc.SizeY = 0.3f;
		shapeDesc.SizeZ = 0.2f;
		building01->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		building01->AddComponent<PhysicsBody>(bodyDesc);
	}

	//Building02
	struct Building02Data
	{
		Vector3 position;
		float yRotation; // Yé≤âÒì]ÅiìxÅj
	};
	ModelPrefab building02Prefab{ "model/building02.glb" };
	std::vector<Building02Data> building02List =
	{
	};

	for (const auto& data : building02List)
	{
		GameObject* building02 = Instantiate(building02Prefab);

		building02->transform().scale() = { 30.0f,50.0f,30.0f };
		building02->transform().position() = data.position;

		XMVECTOR q = XMQuaternionRotationRollPitchYaw(
			0.0f,
			XMConvertToRadians(data.yRotation),
			0.0f
		);
		building02->transform().rotation().FromXMVECTOR(q);

		BoxColliderDesc shapeDesc{};
		shapeDesc.SizeX = 0.1f;
		shapeDesc.SizeY = 1.0f;
		shapeDesc.SizeZ = 0.1f;
		building02->AddComponent<ColliderShape>(shapeDesc);

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Type = BodyType::STATIC;
		building02->AddComponent<PhysicsBody>(bodyDesc);
	}
}
