#include "Player.h"

void PlayerState_Electric::Enter(Player& player)
{
	// 剛体を無効化
	player.physicsBody->SetEnable(false);

	// モデルを電気形態に設定
	//player.meshRenderer->SetModel(player.electricModel);

	// 移動コンポーネント取得
	PlayerMovement* movement = player.movement;

	// 変身コンポーネント取得
	PlayerMorphSystem* morphSystem = player.morphSystem;

	// 電線上にスナップ
	movement->SnapToPowerLine(morphSystem->GetNearestPowerLineID());

	PlayerState::Enter(player);
}

void PlayerState_Electric::HandleInput(Player& player)
{

	// 入力システム取得
	InputHandler* inputHandler = player.inputHandler;

	// 移動コンポーネント取得
	PlayerMovement* movement = player.movement;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = player.stateMachine;

	// ジャンプ・変身コマンドが発行されたら射出・変身処理
	if (inputHandler->IsIssued<PlayerCommand_Jump>()
		|| inputHandler->IsIssued<PlayerCommand_Morph>())
	{
		// 電線から射出
		movement->Eject(
			inputHandler->GetValue<PlayerCommand_MoveX>(),
			inputHandler->GetValue<PlayerCommand_MoveZ>());

		// 人間形態へ変身
		stateMachine->ChangeState(&PlayerStates::HumanMidAir, player);
		return;
	}

	// 移動コマンドが発行されたら方向転換処理
	if (inputHandler->IsIssued<PlayerCommand_MoveX>()
		|| inputHandler->IsIssued<PlayerCommand_MoveZ>())
	{
		// 電線上方向指定処理
		movement->Turn(
			inputHandler->GetValue<PlayerCommand_MoveX>(),
			inputHandler->GetValue<PlayerCommand_MoveZ>());
	}

	PlayerState::HandleInput(player);
}

void PlayerState_Electric::Update(Player& player)
{
	// 移動コンポーネント取得
	PlayerMovement* movement = player.movement;

	// 電線上移動処理
	movement->LineMove();

	PlayerState::Update(player);
}
