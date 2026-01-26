#include "Player.h"

void PlayerState_Electric::Enter(Player& player)
{
	// モデルを電気形態に設定
	player.m_MeshRenderer->SetModel(player.m_ElectricModel);

	// 移動コンポーネント取得
	PlayerMovement* movement = player.m_Movement;

	// 変身コンポーネント取得
	PlayerMorphSystem* morphSystem = player.m_MorphSystem;

	// 電線上にスナップ
	movement->SnapToPowerLine(morphSystem->GetNearestPowerLineID());

	PlayerState::Enter(player);
}

void PlayerState_Electric::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem* inputSystem = player.m_InputSystem;

	// 移動コンポーネント取得
	PlayerMovement* movement = player.m_Movement;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = player.m_StateMachine;

	// ジャンプコマンドが発行されたら射出・変身処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
	{
		// 電線から射出
		movement->Eject(
			inputSystem->GetValue<PlayerCommand_MoveX>(),
			inputSystem->GetValue<PlayerCommand_MoveZ>());

		// 人間形態へ変身
		stateMachine->ChangeState(PlayerStates::HumanMidAir, player);
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

	PlayerState::HandleInput(player);
}

void PlayerState_Electric::Update(Player& player)
{
	// 移動コンポーネント取得
	PlayerMovement* movement = player.m_Movement;

	// 電線上移動処理
	movement->LineMove();

	PlayerState::Update(player);
}
