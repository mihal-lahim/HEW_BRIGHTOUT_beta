#include "player_state.h"
#include "player.h"

void PlayerStateMachine::ChangeState(Player& player, PlayerState* newState)
{
	// 現在のステートから抜ける処理
	m_CurrentState = newState;
	if (m_CurrentState)
	{
		// 新しいステートに入る処理
		m_CurrentState->Enter(player);
	}
}

void PlayerStateMachine::Update(Player& player, double elapsedTime)
{
	if (m_CurrentState)
	{
		// 現在のステートの入力処理
		m_CurrentState->HandleInput(player);
		// 現在のステートの更新処理
		m_CurrentState->Update(player, elapsedTime);
	}
}

void PlayerStateMachine::Draw(const Player& player) const
{
	if (m_CurrentState)
	{
		// 現在のステートの描画処理
		m_CurrentState->Draw(player);
	}
}



void PlayerState::Enter(Player&)
{
}

void PlayerState::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem* inputSystem = player.GetInputSystem();

	// カメラ取得
	TPSCamera* camera = player.GetTPSCamera();

	// カメラ回転処理
	if (inputSystem->IsIssued<PlayerCommand_CameraMoveX>()
		|| inputSystem->IsIssued<PlayerCommand_CameraMoveY>())
	{
		camera->Rotate(
			inputSystem->GetValue<PlayerCommand_CameraMoveX>(),
			inputSystem->GetValue<PlayerCommand_CameraMoveY>());
	}
}

void PlayerState::Update(Player&, double)
{
}

void PlayerState::Draw(const Player& player) const
{
	// プレイヤー描画
	player.Draw();
}
