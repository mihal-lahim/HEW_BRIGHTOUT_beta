#include "player_state.h"
#include "player.h"
#include "tps_camera.h"

void PlayerStateMachine::ChangeState(PlayerState* newState)
{
	// 現在のステートから抜ける処理
	m_CurrentState = newState;

	if (m_CurrentState)
	{
		// 新しいステートに入る処理
		m_CurrentState->Enter();
	}
}

void PlayerStateMachine::Update(double elapsedTime)
{
	if (m_CurrentState)
	{
		// 現在のステートの入力処理
		m_CurrentState->HandleInput();
		// 現在のステートの更新処理
		m_CurrentState->Update(elapsedTime);
	}
}



void PlayerState::Enter()
{}

void PlayerState::HandleInput()
{
	// 入力システム取得
	const InputSystem* inputSystem = GetOwner()->GetComponent<InputSystem>();

	// カメラ取得
	TPSCamera* camera = GetOwner()->GetComponent<TPSCamera>();

	// カメラ回転処理
	if (inputSystem->IsIssued<PlayerCommand_CameraMoveX>()
		|| inputSystem->IsIssued<PlayerCommand_CameraMoveY>())
	{
		camera->Rotate(
			inputSystem->GetValue<PlayerCommand_CameraMoveX>(),
			inputSystem->GetValue<PlayerCommand_CameraMoveY>());
	}
}

void PlayerState::Update(double)
{
}
