
#include "player_state_electric.h"
#include "player.h"

void PlayerState_Electric::Enter(Player& player)
{
	// モデルを電気形態に設定
	player.SetModel(player.GetElectricModel());

	// 移動コンポーネント取得
	PlayerMovement* movement = player.GetMovement();

	// 変身コンポーネント取得
	PlayerMorphSystem* morphSystem = player.GetMorphSystem();

	// 電線上にスナップ
	movement->SnapToPowerLine(morphSystem->GetNearestPowerLineID(player), player);

	PlayerState::Enter(player);
}

void PlayerState_Electric::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem* inputSystem = player.GetInputSystem();

	// 移動コンポーネント取得
	PlayerMovement* movement = player.GetMovement();

	// ジャンプコマンドが発行されたら射出・変身処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
	{
		// 電線から射出
		movement->Eject(
			inputSystem->GetValue<PlayerCommand_MoveX>(),
			inputSystem->GetValue<PlayerCommand_MoveZ>(),
			&player);

		// 人間形態へ変身
		player.ChangeState(&PlayerStates::HumanMidAir);
		return;
	}

	// 移動コマンドが発行されたら方向転換処理
	if (inputSystem->IsIssued<PlayerCommand_MoveX>()
		|| inputSystem->IsIssued<PlayerCommand_MoveZ>())
	{
		// 電線上方向指定処理
		movement->Turn(
			inputSystem->GetValue<PlayerCommand_MoveX>(),
			inputSystem->GetValue<PlayerCommand_MoveZ>(),
			&player);
	}

	PlayerState::HandleInput(player);
}

void PlayerState_Electric::Update(Player& player, double elapsedTime)
{
	// 電線上移動処理
	player.GetMovement()->Move(&player);

	PlayerState::Update(player, elapsedTime);
}

void PlayerState_Electric::Draw(const Player& player) const
{
	PlayerState::Draw(player);
}
