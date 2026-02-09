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



	const auto child = modelObject->GetChildren();

	const auto child2 = child[0]->GetChildren();

	auto* renderer = child2[0]->GetComponent<MeshRenderer>();
	renderer->material.SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
}

void Player::Update()
{
	// 状態管理コンポーネント更新
	stateMachine->Update(*this);
}
