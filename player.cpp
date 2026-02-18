#include "Player.h"
#include "Renderer.h"
#include "InputSystem.h"
#include "Camera.h"
#include "PhysicsBody.h"
#include "ColliderShape.h"

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
