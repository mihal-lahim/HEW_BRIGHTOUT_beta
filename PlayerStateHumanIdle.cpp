
#include "Player.h"

void PlayerState_Human_Idle::Enter(Player& player)
{
	PlayerState_Human::Enter(player);
}

void PlayerState_Human_Idle::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem* inputSystem = player.m_InputSystem;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = player.m_StateMachine;

	// 移動コマンドが発行されたら歩行状態へ遷移
	if (inputSystem->IsIssued<PlayerCommand_MoveX>()
		|| inputSystem->IsIssued<PlayerCommand_MoveZ>())
	{
		stateMachine->ChangeState(PlayerStates::HumanWalk, player);
		return;
	}

	PlayerState_Human::HandleInput(player);
}

void PlayerState_Human_Idle::Update(Player& player)
{

	PlayerState_Human::Update(player);
}