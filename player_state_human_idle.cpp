#include "player_state_human_idle.h"
#include "player_state_human_walk.h"
#include "player_state_human_midair.h"
#include "player.h"

void PlayerState_Human_Idle::Enter()
{
	PlayerState_Human::Enter();
}

void PlayerState_Human_Idle::HandleInput()
{
	PlayerState_Human::HandleInput();
}

void PlayerState_Human_Idle::Update(double elapsedTime)
{
	// 入力システム取得
	const InputSystem* inputSystem = GetOwner()->GetComponent<InputSystem>();

	// ステートマシン取得
	PlayerStateMachine* stateMachine = GetOwner()->GetComponent<PlayerStateMachine>();

	// 移動コマンドが発行されたら歩行状態へ遷移
	if (inputSystem->IsIssued<PlayerCommand_MoveX>() 
		|| inputSystem->IsIssued<PlayerCommand_MoveZ>())
	{
		stateMachine->ChangeState(GetOwner()->GetComponent<PlayerState_Human_Walk>());
		return;
	}

	// ジャンプコマンドが発行されたら空中状態へ遷移
	if (inputSystem->GetValue<PlayerCommand_Jump>())
	{
		stateMachine->ChangeState(GetOwner()->GetComponent<PlayerState_Human_MidAir>());
		return;
	}

	PlayerState_Human::Update(elapsedTime);
}