
#include "player.h"
#include "ObjectManager.h"

Player::Player(InputSystem* inputSystem, PoleManager* poleManger)
{
	// InputSystem設定
	m_InputSystem = inputSystem;

	// PoleManager設定
	m_PoleManager = poleManger;

	// TPSCamera作成・設定
	m_Camera = GetOwner()->Create<TPSCamera>(this);

	// Healthコンポーネント作成・設定
	m_Health = GetOwner()->Create<Health>(100.0f);

	// PlayerMovementコンポーネント作成・設定
	m_Movement = GetOwner()->Create<PlayerMovement>();

	// PlayerMorphSystemコンポーネント作成・設定
	m_MorphSystem = GetOwner()->Create<PlayerMorphSystem>();

	// PlayerStateMachineコンポーネント作成・設定
	m_StateMachine = GetOwner()->Create<PlayerStateMachine>(&PlayerStates::HumanIdle);
}

void Player::Update(double elapsedSec)
{
	m_StateMachine->Update(*this, elapsedSec);
}

void Player::Draw() const
{
	m_StateMachine->Draw(*this);
}
