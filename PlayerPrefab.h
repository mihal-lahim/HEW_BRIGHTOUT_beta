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
#include "PlayerAudio.h"
#include "Billboard.h"
#include <string>
#include <vector>

inline void ApplyTextureRecursive(GameObject* gameObject, const std::wstring& texturePath)
{
	if (!gameObject)
		return;

	auto renderers = gameObject->GetComponents<MeshRenderer>();
	for (auto* renderer : renderers)
	{
		renderer->material.texturePath = texturePath;
		renderer->material.texture = nullptr;
		renderer->renderQueue = RenderQueue::Transparent;
		renderer->material.SetColor({ 2.0f, 2.0f, 2.0f, 1.0f });
	}

	auto skinnedRenderers = gameObject->GetComponents<SkinnedMeshRenderer>();
	for (auto* renderer : skinnedRenderers)
	{
		renderer->material.texturePath = texturePath;
		renderer->material.texture = nullptr;
		renderer->renderQueue = RenderQueue::Transparent;
		renderer->material.SetColor({ 2.0f, 2.0f, 2.0f, 1.0f });
	}

	for (auto* child : gameObject->GetChildren())
	{
		ApplyTextureRecursive(child, texturePath);
	}
}

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
		std::vector<std::string> idleModelPaths =
		{
			"model/P_taiki.fbx"
		};
		std::vector<std::string> walkModelPaths =
		{
			"model/run_01.fbx",
			"model/run_02.fbx",
			"model/run_03.fbx",
			"model/run_04.fbx"
		};
		std::vector<std::string> fireModelPaths =
		{
			"model/P_kougeki_01.fbx",
			"model/P_kougeki_02.fbx"
		};
		GameObject* modelRoot = gameObject.CreateGameObject();
		gameObject.SetChild(*modelRoot);
		player->modelObject = modelRoot;
		player->idleModelObjects.clear();
		player->walkModelObjects.clear();
		player->fireModelObjects.clear();
		player->idleModelObjects.reserve(idleModelPaths.size());
		player->walkModelObjects.reserve(walkModelPaths.size());
		player->fireModelObjects.reserve(fireModelPaths.size());

		auto instantiatePseudoAnimationModels = [&](const std::vector<std::string>& modelPaths, std::vector<GameObject*>& outModels)
		{
			for (size_t i = 0; i < modelPaths.size(); ++i)
			{
				ModelPrefab modelPrefab{ modelPaths[i] };
				GameObject* modelObject = modelRoot->Instantiate(modelPrefab);
				modelRoot->SetChild(*modelObject);
				modelObject->SetActive(false);
				outModels.push_back(modelObject);
			}
		};

		instantiatePseudoAnimationModels(idleModelPaths, player->idleModelObjects);
		instantiatePseudoAnimationModels(walkModelPaths, player->walkModelObjects);
		instantiatePseudoAnimationModels(fireModelPaths, player->fireModelObjects);

		player->walkAnimationInterval = 0.1f;
		if (!player->idleModelObjects.empty())
		{
			player->idleModelObjects.front()->SetActive(true);
		}
		else if (!player->walkModelObjects.empty())
		{
			player->walkModelObjects.front()->SetActive(true);
		}
		modelRoot->transform().scale() = Vector3(0.01f, 0.01f, 0.01f);
		modelRoot->transform().rotation() = Quaternion::SetEulerY(180.0f);

		GameObject* electricEffectObject = gameObject.CreateGameObject();
		gameObject.SetChild(*electricEffectObject);
		// MeshRenderer に Quad メッシュを直接割り当て
		auto* renderer = electricEffectObject->AddComponent<MeshRenderer>();
		renderer->material.texturePath = L"texture/ball.png";
		renderer->material.texture = nullptr;
		renderer->material.SetColor({ 2.0f, 2.0f, 2.0f, 1.0f });
		// Billboard コンポーネントが Awake() でシェーダーとメッシュを自動設定
		electricEffectObject->AddComponent<Billboard>();
		electricEffectObject->transform().scale() = Vector3(1.5f, 1.5f, 1.5f);
		electricEffectObject->transform().position() = Vector3(0.0f, 0.5f, 0.0f);
		electricEffectObject->SetActive(false);
		player->electricEffectObject = electricEffectObject;
		player->electricAnimationInterval = 0.08f;
		player->electricSheetColumns = 6;
		player->electricSheetRows = 6;
		player->electricSheetFrameCount = 31;

		// Healthコンポーネント設定
		gameObject.AddComponent<Health>(100.0f);


		// PlayerMovementコンポーネント設定
		player->movement = gameObject.AddComponent<PlayerMovement>(cam);

		// PlayerAudio 追加
		gameObject.AddComponent<PlayerAudio>();

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
