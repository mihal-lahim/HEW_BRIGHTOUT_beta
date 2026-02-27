#ifndef ENEMY_PREFAB_H
#define ENEMY_PREFAB_H

#include "Prefab.h"
#include "Enemy.h"
#include "Model.h"
#include "GameObject.h"
#include "Quaternion.h"
#include "ColliderShape.h"
#include "PhysicsBody.h"
#include <string>
#include <vector>
#include <exception>
#include <windows.h>

class EnemyPrefab : public Prefab
{
public:
	std::vector<std::string> IdleModelPaths =
	{
		"model/model_dake.fbx"
	};
	std::vector<std::string> MoveModelPaths =
	{
		"model/model_dake.fbx"
	};
	std::vector<std::string> DeadModelPaths =
	{
		"model/model_dake.fbx",

	};
	float IdleAnimationInterval = 0.35f;
	float MoveAnimationInterval = 0.12f;
	float DeadAnimationInterval = 0.1f;
	float DeadDuration = 0.5f;
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
		enemy->IdleAnimationInterval = IdleAnimationInterval;
		enemy->MoveAnimationInterval = MoveAnimationInterval;
		enemy->DeadAnimationInterval = DeadAnimationInterval;
		enemy->DeadDuration = DeadDuration;
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
