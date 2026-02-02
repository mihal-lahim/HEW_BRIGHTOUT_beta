
#include "Player.h"
#include "MeshRenderer.h"
#include "InputSystem.h"
#include "model.h"
#include "Camera.h"
#include "PhysicsBody.h"
#include "ColliderShape.h"

using namespace DirectX;

Player::Player()
{
	//m_HumanModel = ModelLoad("model/kirby.fbx");
}

Player::~Player()
{
	//ModelRelease(m_HumanModel);
}

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
