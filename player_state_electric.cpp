#include "player_state_electric.h"
#include "player_state_human_midair.h"
#include "player.h"
#include "PlayerSystem.h"

void PlayerState_Electric::Enter(PlayerSystem& playerSystem)
{
	// モデルを電気形態に設定
	playerSystem.m_MeshRenderer->SetModel(playerSystem.m_ElectricModel);

	// 移動コンポーネント取得
	PlayerMovement* movement = playerSystem.m_Movement;

	// 変身コンポーネント取得
	PlayerMorphSystem* morphSystem = playerSystem.m_MorphSystem;

	// 電線上にスナップ
	movement->SnapToPowerLine(morphSystem->GetNearestPowerLineID());

	PlayerState::Enter(playerSystem);
}

void PlayerState_Electric::HandleInput(PlayerSystem& playerSystem)
{
	// 入力システム取得
	const InputSystem* inputSystem = playerSystem.m_InputSystem;

	// 移動コンポーネント取得
	PlayerMovement* movement = playerSystem.m_Movement;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = playerSystem.m_StateMachine;

	// ジャンプコマンドが発行されたら射出・変身処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
	{
		// 電線から射出
		movement->Eject(
			inputSystem->GetValue<PlayerCommand_MoveX>(),
			inputSystem->GetValue<PlayerCommand_MoveZ>());

		// 人間形態へ変身
		stateMachine->ChangeState(PlayerStates::HumanMidAir, playerSystem);
		return;
	}

	// 移動コマンドが発行されたら方向転換処理
	if (inputSystem->IsIssued<PlayerCommand_MoveX>()
		|| inputSystem->IsIssued<PlayerCommand_MoveZ>())
	{
		// 電線上方向指定処理
		movement->Turn(
			inputSystem->GetValue<PlayerCommand_MoveX>(),
			inputSystem->GetValue<PlayerCommand_MoveZ>());
	}

	PlayerState::HandleInput(playerSystem);
}

void PlayerState_Electric::Update(double elapsedTime, PlayerSystem& playerSystem)
{
	// 移動コンポーネント取得
	PlayerMovement* movement = playerSystem.m_Movement;

	// 電線上移動処理
	movement->LineMove();

	PlayerState::Update(elapsedTime, playerSystem);
}
