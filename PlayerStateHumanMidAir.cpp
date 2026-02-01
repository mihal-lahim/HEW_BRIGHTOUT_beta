
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

	// 入力値取得
	float inputX = inputHandler->GetValue<PlayerCommand_MoveX>();
	float inputZ = inputHandler->GetValue<PlayerCommand_MoveZ>();

	// 電気ジャンプ処理
	if (inputHandler->IsIssued<PlayerCommand_Jump>())
		movement->ElectricJump(inputX, inputZ);


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