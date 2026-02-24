#include "Player.h"
#include "Renderer.h"
#include "InputSystem.h"
#include "Camera.h"
#include "PhysicsBody.h"
#include "ColliderShape.h"
#include "Model.h"
#include "Bullet.h"
#include "Time.h"

using namespace DirectX;


void Player::Start()
{
	// 初期状態設定
	stateMachine->ChangeState(&PlayerStates::HumanIdle, *this);
	ResetWalkAnimation();
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

static void SetActiveRecursive(GameObject* obj, bool active)
{
	if (!obj) return;
	obj->SetActive(active);
	for (auto* child : obj->GetChildren())
	{
		SetActiveRecursive(child, active);
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
