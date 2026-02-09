#ifndef PLAYER_PREFAB_H
#define PLAYER_PREFAB_H

#include "Prefab.h"
#include "Player.h"
#include "InputSystem.h"
#include "ColliderShape.h"
#include "PhysicsBody.h"
#include "Renderer.h"
#include "Camera.h"
#include "Texture.h"
#include "GameObject.h"

class PlayerPrefab : public Prefab
{
public:
	PlayerPrefab() = default;
	virtual ~PlayerPrefab() = default;
	// インスタンス化メソッド
	virtual void Instantiate(GameObject& gameObject) override
	{
		// Playerコンポーネント作成
		auto* player = gameObject.AddComponent<Player>();
		gameObject.SetTag("Player");

		// TPSCamera作成
		GameObject* cameraObject = gameObject.CreateGameObject();
		auto* cam = cameraObject->AddComponent<Camera>();
		player->camera = cameraObject->AddComponent<TPSCamera>(&gameObject);
		cameraObject->SetTag("MainCamera");

		// プレイヤー用コマンドセット作成
		auto* commandSet = gameObject.AddComponent<PlayerCommandSet>();

		// InputSystem設定
		player->inputHandler = gameObject.AddComponent<InputHandler>(&gameObject.input().gamePad(), commandSet);



		// Healthコンポーネント設定
		gameObject.AddComponent<Health>(100.0f);


		// PlayerMovementコンポーネント設定
		player->movement = gameObject.AddComponent<PlayerMovement>(cam);

		// PlayerMorphSystemコンポーネント設定
		player->morphSystem = gameObject.AddComponent<PlayerMorphSystem>();


		// PlayerStateMachineコンポーネント設定
		player->stateMachine = gameObject.AddComponent<PlayerStateMachine>();

		// モデルオブジェクト設定
		CubePrefab cubePrefab{};
		player->modelObject = gameObject.Instantiate(cubePrefab);
		gameObject.SetChild(*player->modelObject);


		// ColliderShape設定
		CapsuleColliderDesc shapeDesc{};
		shapeDesc.Radius = 0.5f;
		shapeDesc.Height = 1.0f;
		gameObject.AddComponent<ColliderShape>(shapeDesc);


		// PhysicsBody設定
		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Mass = 1.0f;
		bodyDesc.Type = BodyType::DYNAMIC;
		bodyDesc.FixedRotation = { 1.0f, 0.0f, 1.0f };
		player->physicsBody = gameObject.AddComponent<PhysicsBody>(bodyDesc);
	}

};

#endif
