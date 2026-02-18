
#include "Player.h"


void PlayerState_Human_MidAir::Enter(Player& player)
{
	PlayerState_Human::Enter(player);
}

void PlayerState_Human_MidAir::HandleInput(Player& player)
{
	// 入力システム取得
	InputHandler* inputHandler = player.inputHandler;

	// 移動コンポーネント取得
	PlayerMovement* movement = player.movement;

	// 空中移動処理
	if (inputHandler->IsIssued<PlayerCommand_MoveX>()
		|| inputHandler->IsIssued<PlayerCommand_MoveZ>())
	{
		movement->AirMove(
			inputHandler->GetValue<PlayerCommand_MoveX>(),
			inputHandler->GetValue<PlayerCommand_MoveZ>());
	}

	// 電気ジャンプ処理は無効化


	PlayerState_Human::HandleInput(player);
}

void PlayerState_Human_MidAir::Update(Player& player)
{
	// 地上判定
	if (player.movement->IsOnGround())
	{
		player.stateMachine->ChangeState(&PlayerStates::HumanIdle, player);
		return;
	}

	// 重力適用
	player.movement->ApplyGravity();

	PlayerState_Human::Update(player);
}