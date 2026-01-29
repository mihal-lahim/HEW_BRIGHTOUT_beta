
#include "Player.h"
#include "DebugCounter.h"


void PlayerState_Human_Idle::Enter(Player& player)
{
	PlayerState_Human_Ground::Enter(player);
}

void PlayerState_Human_Idle::HandleInput(Player& player)
{

	// 入力システム取得
	const InputSystem* inputSystem = player.inputSystem;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = player.stateMachine;

	// 移動コマンドが発行されたら歩行状態へ遷移
	if (inputSystem->IsIssued<PlayerCommand_MoveX>()
		|| inputSystem->IsIssued<PlayerCommand_MoveZ>())
	{
		stateMachine->ChangeState(&PlayerStates::HumanWalk, player);
		return;
	}


	PlayerState_Human_Ground::HandleInput(player);
}

void PlayerState_Human_Idle::Update(Player& player)
{
	PlayerState_Human_Ground::Update(player);
}