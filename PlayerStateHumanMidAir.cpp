
#include "Player.h"


void PlayerState_Human_MidAir::Enter(Player& player)
{
	PlayerState_Human::Enter(player);
}

void PlayerState_Human_MidAir::HandleInput(Player& player)
{

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