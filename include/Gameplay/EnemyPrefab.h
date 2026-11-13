#ifndef ENEMY_PREFAB_H
#define ENEMY_PREFAB_H

#include "Prefab.h"
#include "Enemy.h"
#include "Model.h"
#include "GameObject.h"
#include "Quaternion.h"
#include "ColliderShape.h"
#include "PhysicsBody.h"
#include "Renderer.h"
#include "Billboard.h"
#include <string>
#include <vector>
#include <exception>
#include <windows.h>

class EnemyPrefab : public Prefab
{
public:
	std::vector<std::string> IdleModelPaths =
	{
		"model/E_kougeki_01.fbx"
	};
	std::vector<std::string> MoveModelPaths =
	{
		"model/E_run_01.fbx",
		"model/E_run_02.fbx",
		"model/E_run_03.fbx",
		"model/E_run_04.fbx",
		"model/E_run_05.fbx",
	};
	std::vector<std::string> AttackModelPaths =
	{
		"model/E_kougeki_01.fbx",
		"model/E_kougeki_02.fbx",
		"model/E_kougeki_03.fbx",
		"model/E_kougeki_04.fbx",
		"model/E_kougeki_05.fbx"
	};
	std::vector<std::string> DeadModelPaths =
	{
		"model/E_HP0_01.fbx",
		"model/E_HP0_02.fbx",
		"model/E_HP0_03.fbx",

	};
	float IdleAnimationInterval = 0.35f;
	float MoveAnimationInterval = 0.12f;
	float AttackAnimationInterval = 0.12f;
	float DeadAnimationInterval = 0.1f;
	float DeadDuration = 0.5f;
	float AttackRange = 1.0f;
	float AttackDamage = 10.0f;
	float AttackInterval = 2.0f;
	std::wstring AttackEffectTexturePath = L"texture/kiru_effects_1.png";
	Vector3 AttackEffectOffset = Vector3(0.0f, 0.0f, 0.6f);
	Vector3 AttackEffectScale = Vector3(0.8f, 0.8f, 0.8f);
	float AttackEffectPitch = 180.0f;
	float AttackEffectDuration = 0.2f;
	int AttackEffectDirectionColumns = 1;
	int AttackEffectDirectionRows = 1;
	int AttackEffectDirectionFrameCount = 1;
	float AttackEffectDirectionYawOffset = 0.0f;
	bool UseGravity = true;
	float Gravity = -30.0f;

	EnemyPrefab() = default;
	virtual ~EnemyPrefab() = default;

	void Instantiate(GameObject& gameObject) override
	{
		auto* enemy = gameObject.AddComponent<Enemy>();
		gameObject.SetTag("Enemy");

		GameObject* modelRoot = gameObject.CreateGameObject();
		gameObject.SetChild(*modelRoot);
		enemy->modelObject = modelRoot;
		enemy->idleModelObjects.clear();
		enemy->moveModelObjects.clear();
		enemy->attackModelObjects.clear();
		enemy->deadModelObjects.clear();

		auto instantiatePseudoAnimationModels = [&](const std::vector<std::string>& modelPaths, std::vector<GameObject*>& outModels)
		{
			for (size_t i = 0; i < modelPaths.size(); ++i)
			{
				try
				{
					ModelPrefab modelPrefab{ modelPaths[i] };
					GameObject* modelObject = modelRoot->Instantiate(modelPrefab);
					modelRoot->SetChild(*modelObject);
					modelObject->SetActive(false);
					outModels.push_back(modelObject);
				}
				catch (const std::exception& ex)
				{
					std::string msg = "[EnemyPrefab] model load failed: " + modelPaths[i] + " / " + ex.what() + "\n";
					OutputDebugStringA(msg.c_str());
				}
				catch (...)
				{
					std::string msg = "[EnemyPrefab] model load failed: " + modelPaths[i] + " / unknown exception\n";
					OutputDebugStringA(msg.c_str());
				}
			}

			if (outModels.empty())
			{
				ModelPrefab fallbackPrefab{ "model/Cube.glb" };
				GameObject* fallbackObject = modelRoot->Instantiate(fallbackPrefab);
				modelRoot->SetChild(*fallbackObject);
				fallbackObject->SetActive(false);
				outModels.push_back(fallbackObject);
			}
		};

		instantiatePseudoAnimationModels(IdleModelPaths, enemy->idleModelObjects);
		instantiatePseudoAnimationModels(MoveModelPaths, enemy->moveModelObjects);
		instantiatePseudoAnimationModels(AttackModelPaths, enemy->attackModelObjects);
		instantiatePseudoAnimationModels(DeadModelPaths, enemy->deadModelObjects);

		if (!enemy->idleModelObjects.empty())
		{
			enemy->idleModelObjects.front()->SetActive(true);
		}
		else if (!enemy->moveModelObjects.empty())
		{
			enemy->moveModelObjects.front()->SetActive(true);
		}

		modelRoot->transform().scale() = Vector3(0.01f, 0.01f, 0.01f);
		modelRoot->transform().rotation() = Quaternion::SetEulerY(180.0f);

		GameObject* attackEffectObject = gameObject.CreateGameObject();
		auto* attackEffectRenderer = attackEffectObject->AddComponent<MeshRenderer>();
		attackEffectRenderer->material.texturePath = AttackEffectTexturePath;
		attackEffectRenderer->material.texture = nullptr;
		attackEffectRenderer->material.SetColor({ 2.0f, 2.0f, 2.0f, 1.0f });
		attackEffectObject->AddComponent<Billboard>();
		attackEffectObject->transform().position() = gameObject.transform().position() + AttackEffectOffset;
		attackEffectObject->transform().rotation() = Quaternion::SetEulerX(AttackEffectPitch);
		attackEffectObject->transform().scale() = AttackEffectScale;
		attackEffectObject->SetActive(false);
		enemy->attackEffectObject = attackEffectObject;
		enemy->AttackEffectDuration = AttackEffectDuration;
		enemy->AttackEffectForwardDistance = AttackEffectOffset.z;
		enemy->AttackEffectHeightOffset = AttackEffectOffset.y;
		enemy->AttackEffectPitch = AttackEffectPitch;
		enemy->AttackEffectDirectionColumns = AttackEffectDirectionColumns;
		enemy->AttackEffectDirectionRows = AttackEffectDirectionRows;
		enemy->AttackEffectDirectionFrameCount = AttackEffectDirectionFrameCount;
		enemy->AttackEffectDirectionYawOffset = AttackEffectDirectionYawOffset;

		enemy->IdleAnimationInterval = IdleAnimationInterval;
		enemy->MoveAnimationInterval = MoveAnimationInterval;
		enemy->AttackAnimationInterval = AttackAnimationInterval;
		enemy->DeadAnimationInterval = DeadAnimationInterval;
		enemy->DeadDuration = DeadDuration;
		enemy->AttackRange = AttackRange;
		enemy->AttackDamage = AttackDamage;
		enemy->AttackInterval = AttackInterval;
		enemy->UseGravity = UseGravity;
		enemy->Gravity = Gravity;

		CapsuleColliderDesc shapeDesc{};
		shapeDesc.Radius = 0.5f;
		shapeDesc.Height = 1.0f;
		gameObject.AddComponent<ColliderShape>(shapeDesc);

		modelRoot->transform().position().y = -(shapeDesc.Radius + (shapeDesc.Height * 0.5f));

		PhysicsBodyDesc bodyDesc{};
		bodyDesc.Mass = 1.0f;
		bodyDesc.Type = BodyType::DYNAMIC;
		bodyDesc.FixedRotation = { 1.0f, 0.0f, 1.0f };
		gameObject.AddComponent<PhysicsBody>(bodyDesc);
	}
};

#endif
