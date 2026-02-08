
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
}

void Player::Update()
{
	// 状態管理コンポーネント更新
	stateMachine->Update(*this);
}
