#include "player_state_human_idle.h"
#include "player_state_human_walk.h"
#include "player_state_human_midair.h"
#include "player.h"
#include "PlayerSystem.h"

void PlayerState_Human_Idle::Enter(PlayerSystem& playerSystem)
{
	PlayerState_Human::Enter(playerSystem);
}

void PlayerState_Human_Idle::HandleInput(PlayerSystem& playerSystem)
{
	// 入力システム取得
	const InputSystem* inputSystem = playerSystem.m_InputSystem;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = playerSystem.m_StateMachine;

	// 移動コマンドが発行されたら歩行状態へ遷移
	if (inputSystem->IsIssued<PlayerCommand_MoveX>()
		|| inputSystem->IsIssued<PlayerCommand_MoveZ>())
	{
		stateMachine->ChangeState(PlayerStates::HumanWalk, playerSystem);
		return;
	}

	PlayerState_Human::HandleInput(playerSystem);
}

void PlayerState_Human_Idle::Update(double elapsedTime, PlayerSystem& playerSystem)
{

	PlayerState_Human::Update(elapsedTime, playerSystem);
}