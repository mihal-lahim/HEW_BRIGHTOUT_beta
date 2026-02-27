#include "Player.h"
#include "Renderer.h"
#include "InputSystem.h"
#include "Camera.h"
#include "PhysicsBody.h"
#include "ColliderShape.h"
#include "Model.h"
#include "Bullet.h"
#include "Title.h"
#include "Time.h"
#include "SceneSystem.h"
#include <algorithm>
#include <cmath>

using namespace DirectX;

static void SetActiveRecursive(GameObject* obj, bool active);
static void SetUVRectRecursive(GameObject* obj, const DirectX::XMFLOAT4& uvRect);


void Player::Start()
{
	// 初期状態設定
	stateMachine->ChangeState(&PlayerStates::HumanIdle, *this);
	ResetWalkAnimation();
	ResetHumanPseudoAnimationGroup(idleModelObjects, idleAnimationIndex, idleAnimationTimer);
	ResetHumanPseudoAnimationGroup(fireModelObjects, fireAnimationIndex, fireAnimationTimer);
	m_HumanPseudoAnimationMode = HumanPseudoAnimationMode::Idle;
	ResetElectricAnimation();
	SetElectricEffectActive(false);
}

void Player::Update()
{
	if (!m_IsReturningToTitle && gameObject().transform().position().y < FallReturnY)
	{
		m_IsReturningToTitle = true;
		scene().ChangeScene<Title>();
		return;
	}

	// 状態管理コンポーネント更新
	stateMachine->Update(*this);
	if (modelObject && modelObject->IsActiveInHierarchy() && inputHandler)
	{
		const float moveX = inputHandler->GetValue<PlayerCommand_MoveX>();
		const float moveZ = inputHandler->GetValue<PlayerCommand_MoveZ>();
		if (std::fabs(moveX) <= 0.01f && std::fabs(moveZ) <= 0.01f)
		{
			ApplyFacingYaw();
		}
	}
	HandleFire();
	UpdateHumanPseudoAnimation((float)Time::DeltaTime());
	UpdateMoveSpeedBuffText((float)Time::DeltaTime());
	UpdateRepairSpeedBuffText((float)Time::DeltaTime());
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
		m_FireAnimationTimer = FireAnimationDuration;
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
	if (!modelObject)
		return;

	if (!active)
	{
		SetActiveRecursive(modelObject, false);
		return;
	}

	modelObject->SetActive(true);
	HideAllHumanPseudoAnimationModels();
	m_HumanPseudoAnimationMode = HumanPseudoAnimationMode::Idle;

	if (!idleModelObjects.empty())
	{
		ResetHumanPseudoAnimationGroup(idleModelObjects, idleAnimationIndex, idleAnimationTimer);
	}
	else
	{
		ResetHumanPseudoAnimationGroup(walkModelObjects, walkAnimationIndex, walkAnimationTimer);
		m_HumanPseudoAnimationMode = HumanPseudoAnimationMode::Walk;
	}
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

void Player::SetFacingYaw(float yaw)
{
	m_LastFacingYaw = yaw;
}

void Player::ApplyFacingYaw()
{
	gameObject().transform().rotation() = Quaternion::SetEulerY(m_LastFacingYaw);
	if (physicsBody && physicsBody->IsEnable())
	{
		physicsBody->SyncTransformToGameObject();
	}
}

void Player::HideAllHumanPseudoAnimationModels()
{
	for (auto* model : walkModelObjects)
	{
		if (model) SetActiveRecursive(model, false);
	}
	for (auto* model : idleModelObjects)
	{
		if (model) SetActiveRecursive(model, false);
	}
	for (auto* model : fireModelObjects)
	{
		if (model) SetActiveRecursive(model, false);
	}
}

void Player::ResetHumanPseudoAnimationGroup(std::vector<GameObject*>& models, size_t& index, float& timer)
{
	timer = 0.0f;
	index = 0;
	for (size_t i = 0; i < models.size(); ++i)
	{
		if (models[i])
			SetActiveRecursive(models[i], i == 0);
	}
}

void Player::AdvanceHumanPseudoAnimationGroup(std::vector<GameObject*>& models, size_t& index, float& timer, float interval, float deltaTime)
{
	if (models.size() <= 1 || interval <= 0.0f)
		return;

	timer += deltaTime;
	if (timer < interval)
		return;

	timer -= interval;

	if (models[index])
		SetActiveRecursive(models[index], false);

	index = (index + 1) % models.size();

	if (models[index])
		SetActiveRecursive(models[index], true);
}

void Player::UpdateHumanPseudoAnimation(float deltaTime)
{
	if (deltaTime <= 0.0f)
		return;

	if (!modelObject || !modelObject->IsActiveInHierarchy())
		return;

	if (m_FireAnimationTimer > 0.0f)
	{
		m_FireAnimationTimer -= deltaTime;
	}

	bool isMovingInput = false;
	if (inputHandler)
	{
		const float moveX = inputHandler->GetValue<PlayerCommand_MoveX>();
		const float moveZ = inputHandler->GetValue<PlayerCommand_MoveZ>();
		isMovingInput = std::fabs(moveX) > 0.01f || std::fabs(moveZ) > 0.01f;
	}

	HumanPseudoAnimationMode nextMode = HumanPseudoAnimationMode::Idle;
	if (m_FireAnimationTimer > 0.0f)
	{
		nextMode = HumanPseudoAnimationMode::Fire;
	}
	else if (isMovingInput)
	{
		nextMode = HumanPseudoAnimationMode::Walk;
	}

	std::vector<GameObject*>* activeModels = &idleModelObjects;
	size_t* activeIndex = &idleAnimationIndex;
	float* activeTimer = &idleAnimationTimer;
	float activeInterval = IdleAnimationInterval;

	if (nextMode == HumanPseudoAnimationMode::Walk)
	{
		activeModels = &walkModelObjects;
		activeIndex = &walkAnimationIndex;
		activeTimer = &walkAnimationTimer;
		activeInterval = walkAnimationInterval;
	}
	else if (nextMode == HumanPseudoAnimationMode::Fire)
	{
		activeModels = &fireModelObjects;
		activeIndex = &fireAnimationIndex;
		activeTimer = &fireAnimationTimer;
		activeInterval = FireAnimationInterval;
	}

	if (activeModels->empty())
	{
		activeModels = &walkModelObjects;
		activeIndex = &walkAnimationIndex;
		activeTimer = &walkAnimationTimer;
		activeInterval = walkAnimationInterval;
		nextMode = HumanPseudoAnimationMode::Walk;
	}

	if (activeModels->empty())
		return;

	if (m_HumanPseudoAnimationMode != nextMode)
	{
		HideAllHumanPseudoAnimationModels();
		ResetHumanPseudoAnimationGroup(*activeModels, *activeIndex, *activeTimer);
		m_HumanPseudoAnimationMode = nextMode;
	}

	AdvanceHumanPseudoAnimationGroup(*activeModels, *activeIndex, *activeTimer, activeInterval, deltaTime);
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

void Player::ApplyPermanentMoveSpeedBuff()
{
	if (!movement)
		return;

	PlayerMoveCtx moveCtx = movement->GetMoveCtx();
	moveCtx.WalkSpeed += MoveSpeedBuffPerRestore;
	movement->SetMoveCtx(moveCtx);
	++MoveSpeedBuffStack;
	ShowMoveSpeedBuffText();
}

void Player::ApplyPermanentRestoreSpeedBuff()
{
	++RestoreSpeedBuffStack;
	ShowRepairSpeedBuffText();
}

float Player::GetRestoreSpeedMultiplier() const
{
	return 1.0f + (RestoreSpeedBuffPerRestore * (float)RestoreSpeedBuffStack);
}

void Player::ShowMoveSpeedBuffText()
{
	m_MoveSpeedBuffTextTimer = MoveSpeedBuffTextDuration;
	if (moveSpeedTextRoot)
		SetActiveRecursive(moveSpeedTextRoot, true);
}

void Player::UpdateMoveSpeedBuffText(float deltaTime)
{
	if (m_MoveSpeedBuffTextTimer <= 0.0f)
	{
		if (moveSpeedTextRoot)
			SetActiveRecursive(moveSpeedTextRoot, false);
		return;
	}

	m_MoveSpeedBuffTextTimer -= deltaTime;

	if (m_MoveSpeedBuffTextTimer <= 0.0f)
	{
		m_MoveSpeedBuffTextTimer = 0.0f;
		if (moveSpeedTextRoot)
			SetActiveRecursive(moveSpeedTextRoot, false);
	}
}

void Player::ShowRepairSpeedBuffText()
{
	m_RepairSpeedBuffTextTimer = RepairSpeedBuffTextDuration;
	if (repairSpeedTextRoot)
		SetActiveRecursive(repairSpeedTextRoot, true);
}

void Player::UpdateRepairSpeedBuffText(float deltaTime)
{
	if (m_RepairSpeedBuffTextTimer <= 0.0f)
	{
		if (repairSpeedTextRoot)
			SetActiveRecursive(repairSpeedTextRoot, false);
		return;
	}

	m_RepairSpeedBuffTextTimer -= deltaTime;

	if (m_RepairSpeedBuffTextTimer <= 0.0f)
	{
		m_RepairSpeedBuffTextTimer = 0.0f;
		if (repairSpeedTextRoot)
			SetActiveRecursive(repairSpeedTextRoot, false);
	}
}
