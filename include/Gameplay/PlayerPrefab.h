#ifndef PLAYER_PREFAB_H
#define PLAYER_PREFAB_H

#include "Prefab.h"
#include "Player.h"
#include "InputSystem.h"
#include "ColliderShape.h"
#include "PhysicsBody.h"
#include "Renderer.h"
#include "RenderingSystem.h"
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


		//player電気状態エフェクト用ビルボード
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

		// 速度バフ表示用ビルボードテキスト
		GameObject* moveSpeedTextRoot = gameObject.CreateGameObject();
		gameObject.SetChild(*moveSpeedTextRoot);
		moveSpeedTextRoot->SetName("MoveSpeedBuffText");
		moveSpeedTextRoot->transform().position() = Vector3(0.0f, 2.2f, 0.0f);
		moveSpeedTextRoot->SetActive(false);
		{
			const char* text = "spped up!";
			const int textLen = 9;
			const float charScale = 0.35f;
			const float spacing = 0.38f;
			const float startX = -((float)(textLen - 1)) * spacing * 0.5f;

			for (int ci = 0; ci < textLen; ++ci)
			{
				GameObject* charObj = moveSpeedTextRoot->CreateGameObject();
				moveSpeedTextRoot->SetChild(*charObj);
				charObj->transform().position() = Vector3(startX + spacing * (float)ci, 0.0f, 0.0f);
				charObj->transform().scale() = Vector3(charScale, charScale, charScale);

				auto* mr = charObj->AddComponent<MeshRenderer>();
				mr->mesh = gameObject.rendering().CreateBillboardQuad().get();
				mr->material.texturePath = L"texture/consolab_ascii_512.png";
				mr->material.texture = nullptr;
				mr->material.vsPath = "BillboardVS.cso";
				mr->material.psPath = "BillboardPS.cso";
				mr->material.shaderProgram = nullptr;
				mr->renderQueue = RenderQueue::Transparent;
				mr->material.SetColor({ 2.0f, 2.0f, 2.0f, 1.0f });

				// DebugText と同じUV計算
				int index = text[ci] - ' ';
				float u0 = (float)(index % 16) / 16.0f;
				float v0 = (float)(index / 16) / 16.0f;
				mr->material.SetFloat4("uv_rect", { u0, v0, 1.0f / 16.0f, 1.0f / 16.0f });
			}
		}
		player->moveSpeedTextRoot = moveSpeedTextRoot;

		// 復旧速度バフ表示用ビルボードテキスト
		GameObject* repairSpeedTextRoot = gameObject.CreateGameObject();
		gameObject.SetChild(*repairSpeedTextRoot);
		repairSpeedTextRoot->SetName("RepairSpeedBuffText");
		repairSpeedTextRoot->transform().position() = Vector3(0.0f, 2.7f, 0.0f);
		repairSpeedTextRoot->SetActive(false);
		{
			const char* text = "repair speed up!";
			const int textLen = 15;
			const float charScale = 0.28f;
			const float spacing = 0.30f;
			const float startX = -((float)(textLen - 1)) * spacing * 0.5f;

			for (int ci = 0; ci < textLen; ++ci)
			{
				GameObject* charObj = repairSpeedTextRoot->CreateGameObject();
				repairSpeedTextRoot->SetChild(*charObj);
				charObj->transform().position() = Vector3(startX + spacing * (float)ci, 0.0f, 0.0f);
				charObj->transform().scale() = Vector3(charScale, charScale, charScale);

				auto* mr = charObj->AddComponent<MeshRenderer>();
				mr->mesh = gameObject.rendering().CreateBillboardQuad().get();
				mr->material.texturePath = L"texture/consolab_ascii_512.png";
				mr->material.texture = nullptr;
				mr->material.vsPath = "BillboardVS.cso";
				mr->material.psPath = "BillboardPS.cso";
				mr->material.shaderProgram = nullptr;
				mr->renderQueue = RenderQueue::Transparent;
				mr->material.SetColor({ 2.0f, 2.0f, 2.0f, 1.0f });

				int index = text[ci] - ' ';
				float u0 = (float)(index % 16) / 16.0f;
				float v0 = (float)(index / 16) / 16.0f;
				mr->material.SetFloat4("uv_rect", { u0, v0, 1.0f / 16.0f, 1.0f / 16.0f });
			}
		}
		player->repairSpeedTextRoot = repairSpeedTextRoot;

		// Health コンポーネント設定
		player->health = gameObject.AddComponent<Health>(100.0f);


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
