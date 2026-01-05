#include "player_state_human_idle.h"
#include "player.h"

void PlayerState_Human_Idle::Enter(Player& player)
{
	PlayerState_Human::Enter(player);
}

void PlayerState_Human_Idle::HandleInput(Player& player)
{
	PlayerState_Human::HandleInput(player);
}

void PlayerState_Human_Idle::Update(Player& player, double elapsedTime)
{
	// 入力システム取得
	const InputSystem* inputSystem = player.GetInputSystem();

	// 移動コマンドが発行されたら歩行状態へ遷移
	if (inputSystem->IsIssued<PlayerCommand_MoveX>() 
		|| inputSystem->IsIssued<PlayerCommand_MoveZ>())
	{
		player.ChangeState(&PlayerStates::HumanWalk);
		return;
	}

	// ジャンプコマンドが発行されたら空中状態へ遷移
	if (inputSystem->GetValue<PlayerCommand_Jump>())
	{
		player.ChangeState(&PlayerStates::HumanMidAir);
		return;
	}

	PlayerState_Human::Update(player, elapsedTime);
}

void PlayerState_Human_Idle::Draw(const Player& player) const
{
	PlayerState_Human::Draw(player);
}