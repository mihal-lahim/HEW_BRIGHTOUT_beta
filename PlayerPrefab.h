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
#include <string>
#include <vector>

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

		// モデル設定
		std::vector<std::string> walkModelPaths =
		{
			"model/run_hidari.fbx",
			"model/run_migi.fbx"
		};
		GameObject* modelRoot = gameObject.CreateGameObject();
		gameObject.SetChild(*modelRoot);
		player->modelObject = modelRoot;
		player->walkModelObjects.clear();
		player->walkModelObjects.reserve(walkModelPaths.size());
		player->walkAnimationInterval = 0.1f;
		for (size_t i = 0; i < walkModelPaths.size(); ++i)
		{
			ModelPrefab modelPrefab{ walkModelPaths[i] };
			GameObject* modelObject = modelRoot->Instantiate(modelPrefab);
			modelRoot->SetChild(*modelObject);
			modelObject->SetActive(i == 0);
			player->walkModelObjects.push_back(modelObject);
		}
		modelRoot->transform().scale() = Vector3(0.01f, 0.01f, 0.01f);
		modelRoot->transform().rotation() = Quaternion::SetEulerY(180.0f);

		// Healthコンポーネント設定
		gameObject.AddComponent<Health>(100.0f);


		// PlayerMovementコンポーネント設定
		player->movement = gameObject.AddComponent<PlayerMovement>(cam);

		// PlayerMorphSystemコンポーネント設定
		player->morphSystem = gameObject.AddComponent<PlayerMorphSystem>();


		// PlayerStateMachineコンポーネント設定
		player->stateMachine = gameObject.AddComponent<PlayerStateMachine>();


		// ColliderShape設定
		CapsuleColliderDesc shapeDesc{};
		shapeDesc.Radius = 0.5f;
		shapeDesc.Height = 1.0f;
		gameObject.AddComponent<ColliderShape>(shapeDesc);

		// モデルの足元を地面に合わせる
		modelRoot->transform().position().y = -(shapeDesc.Radius + (shapeDesc.Height * 0.5f));


		// PhysicsBody設定
		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Mass = 1.0f;
		bodyDesc.Type = BodyType::DYNAMIC;
		bodyDesc.FixedRotation = { 1.0f, 0.0f, 1.0f };
		player->physicsBody = gameObject.AddComponent<PhysicsBody>(bodyDesc);
	}

};

#endif
