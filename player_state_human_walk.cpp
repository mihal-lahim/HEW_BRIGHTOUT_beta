#include "player_state_human_walk.h"
#include "player_state_human_idle.h"
#include "player.h"
#include "player_command.h"
#include "player_state.h"

using namespace DirectX;

void PlayerState_Human_Walk::Enter()
{
	PlayerState_Human_Ground::Enter();
}

void PlayerState_Human_Walk::HandleInput()
{
	// 入力システム取得
	const InputSystem* inputSystem = GetOwner()->GetComponent<InputSystem>();

	// ステートマシン取得
	PlayerStateMachine* stateMachine = GetOwner()->GetComponent<PlayerStateMachine>();

	// 歩行入力がなくなったらアイドル状態へ遷移
	if(inputSystem->IsIssued<PlayerCommand_MoveX>() == 0.0f &&
		inputSystem->IsIssued<PlayerCommand_MoveZ>() == 0.0f)
	{
		stateMachine->ChangeState(GetOwner()->GetComponent<PlayerState_Human_Idle>());
		return;
	}

	PlayerState_Human::HandleInput();
}

void PlayerState_Human_Walk::Update(double elapsedTime)
{
	PlayerState_Human_Ground::Update(elapsedTime);
}