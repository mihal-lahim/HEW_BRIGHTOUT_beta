#include "player_state.h"
#include "player.h"
#include "tps_camera.h"
#include "PlayerSystem.h"

void PlayerStateMachine::ChangeState(PlayerState* newState, PlayerSystem& playerSystem)
{
	// 現在のステートから抜ける処理
	m_CurrentState = newState;

	if (m_CurrentState)
	{
		// 新しいステートに入る処理
		m_CurrentState->Enter(playerSystem);
	}
}

void PlayerStateMachine::Update(double elapsedTime, PlayerSystem& playerSystem)
{
	if (m_CurrentState)
	{
		// 現在のステートの入力処理
		m_CurrentState->HandleInput(playerSystem);
		// 現在のステートの更新処理
		m_CurrentState->Update(elapsedTime, playerSystem);
	}
}



void PlayerState::Enter(PlayerSystem&)
{}

void PlayerState::HandleInput(PlayerSystem& playerSystem)
{
	// 入力システム取得
	const InputSystem* inputSystem = playerSystem.m_InputSystem;

	float inputX = inputSystem->GetValue<PlayerCommand_CameraMoveX>();
	float inputY = inputSystem->GetValue<PlayerCommand_CameraMoveY>();

	// カメラ回転処理
	if (inputX != 0.0f || inputY != 0.0f)
		playerSystem.m_Camera->Rotate(inputX, inputY);
}

void PlayerState::Update(double, PlayerSystem&)
{
}

