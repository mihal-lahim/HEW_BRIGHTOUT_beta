#include "Player.h"

void PlayerStateMachine::ChangeState(PlayerState* newState, Player& player)
{
	// 現在のステートから抜ける処理
	m_CurrentState = newState;

	if (m_CurrentState)
	{
		// 新しいステートに入る処理
		m_CurrentState->Enter(player);
	}
}

void PlayerStateMachine::Update(Player& player)
{
	if (m_CurrentState)
	{
		// 現在のステートの入力処理
		m_CurrentState->HandleInput(player);
		// 現在のステートの更新処理
		m_CurrentState->Update(player);
	}
}



void PlayerState::Enter(Player&)
{}

void PlayerState::HandleInput(Player& player)
{
	// 入力システム取得
	InputHandler* inputHandler = player.inputHandler;

	float inputX = inputHandler->GetValue<PlayerCommand_CameraMoveX>();
	float inputY = inputHandler->GetValue<PlayerCommand_CameraMoveY>();

	// カメラ回転処理
	if (inputX != 0.0f || inputY != 0.0f)
		player.camera->Rotate(inputX, inputY);
}

void PlayerState::Update(Player&)
{
}

