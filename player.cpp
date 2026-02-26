#include "Player.h"
#include "Renderer.h"
#include "InputSystem.h"
#include "Camera.h"
#include "PhysicsBody.h"
#include "ColliderShape.h"
#include "Model.h"
#include "Bullet.h"
#include "Time.h"
#include <algorithm>

using namespace DirectX;

static void SetActiveRecursive(GameObject* obj, bool active);
static void SetUVRectRecursive(GameObject* obj, const DirectX::XMFLOAT4& uvRect);


void Player::Start()
{
	// 初期状態設定
	stateMachine->ChangeState(&PlayerStates::HumanIdle, *this);
	ResetWalkAnimation();
	ResetElectricAnimation();
	SetElectricEffectActive(false);
}

void Player::Update()
{
	// 状態管理コンポーネント更新
	stateMachine->Update(*this);
	HandleFire();
}

void Player::HandleFire()
{
	float dt = (float)Time::DeltaTime();
	if (m_FireTimer > 0.0f)
	{
		m_FireTimer -= dt;
	}

	if (!inputHandler || m_FireTimer > 0.0f)
		return;

	if (inputHandler->IsIssued<PlayerCommand_Fire>())
	{
		m_FireTimer = FireInterval;
		FireBullet();
	}
}

void Player::FireBullet()
{
	Vector3 forward = Vector3(0.0f, 0.0f, 1.0f).Rotate(gameObject().transform().rotation());
	if (forward.IsZero())
	{
		forward = Vector3(0.0f, 0.0f, 1.0f);
	}
	forward = forward.Normalize();

	GameObject* bulletObject = CreateGameObject();
	bulletObject->SetTag("Bullet");
	bulletObject->transform().position() = gameObject().transform().position() + forward * 1.0f;
	bulletObject->transform().rotation() = gameObject().transform().rotation();

	ModelPrefab bulletModel{ "model/cube.glb" };
	GameObject* bulletVisual = bulletObject->Instantiate(bulletModel);
	bulletObject->SetChild(*bulletVisual);
	bulletVisual->transform().scale() = Vector3(0.2f, 0.2f, 0.2f);

	auto* bullet = bulletObject->AddComponent<Bullet>();
	bullet->Direction = forward;
	bullet->Speed = BulletSpeed;
	bullet->LifeTime = BulletLifeTime;
}

void Player::SetHumanVisualActive(bool active)
{
	SetActiveRecursive(modelObject, active);
}

void Player::SetElectricEffectActive(bool active)
{
	SetActiveRecursive(electricEffectObject, active);
}

static void SetActiveRecursive(GameObject* obj, bool active)
{
	if (!obj) return;
	obj->SetActive(active);
	for (auto* child : obj->GetChildren())
	{
		SetActiveRecursive(child, active);
	}
}

static void SetUVRectRecursive(GameObject* obj, const DirectX::XMFLOAT4& uvRect)
{
	if (!obj) return;

	auto renderers = obj->GetComponents<MeshRenderer>();
	for (auto* renderer : renderers)
	{
		renderer->material.SetFloat4("uv_rect", uvRect);
	}

	auto skinnedRenderers = obj->GetComponents<SkinnedMeshRenderer>();
	for (auto* renderer : skinnedRenderers)
	{
		renderer->material.SetFloat4("uv_rect", uvRect);
	}

	for (auto* child : obj->GetChildren())
	{
		SetUVRectRecursive(child, uvRect);
	}
}

void Player::ResetWalkAnimation()
{
	walkAnimationTimer = 0.0f;
	walkAnimationIndex = 0;

	if (walkModelObjects.empty())
		return;

	for (size_t i = 0; i < walkModelObjects.size(); ++i)
	{
		if (walkModelObjects[i])
			SetActiveRecursive(walkModelObjects[i], i == 0);
	}
}

void Player::AdvanceWalkAnimation(float deltaTime)
{
	if (walkModelObjects.size() <= 1 || walkAnimationInterval <= 0.0f)
		return;

	walkAnimationTimer += deltaTime;
	if (walkAnimationTimer < walkAnimationInterval)
		return;

	walkAnimationTimer -= walkAnimationInterval;

	if (walkModelObjects[walkAnimationIndex])
		SetActiveRecursive(walkModelObjects[walkAnimationIndex], false);

	walkAnimationIndex = (walkAnimationIndex + 1) % walkModelObjects.size();

	if (walkModelObjects[walkAnimationIndex])
		SetActiveRecursive(walkModelObjects[walkAnimationIndex], true);
}

void Player::ResetElectricAnimation()
{
	electricAnimationTimer = 0.0f;
	electricAnimationIndex = 0;
	ApplyElectricFrameUV(electricAnimationIndex);
}

void Player::AdvanceElectricAnimation(float deltaTime)
{
	if (electricAnimationInterval <= 0.0f || electricSheetFrameCount <= 1)
		return;

	electricAnimationTimer += deltaTime;
	if (electricAnimationTimer < electricAnimationInterval)
		return;

	electricAnimationTimer -= electricAnimationInterval;
	electricAnimationIndex = (electricAnimationIndex + 1) % (size_t)(std::max)(1, electricSheetFrameCount);
	ApplyElectricFrameUV(electricAnimationIndex);
}

void Player::ApplyElectricFrameUV(size_t frameIndex)
{
	if (!electricEffectObject || electricSheetColumns <= 0 || electricSheetRows <= 0)
		return;

	const int totalFrames = (std::max)(1, electricSheetFrameCount);
	const int frame = (int)(frameIndex % (size_t)totalFrames);
	const int column = frame % electricSheetColumns;
	const int row = frame / electricSheetColumns;

	const float width = 1.0f / (float)electricSheetColumns;
	const float height = 1.0f / (float)electricSheetRows;
	const float offsetX = width * (float)column;
	const float offsetY = height * (float)row;

	SetUVRectRecursive(electricEffectObject, { offsetX, offsetY, width, height });
}
