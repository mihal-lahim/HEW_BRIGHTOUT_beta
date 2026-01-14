#include "player_state_human_walk.h"
#include "player_state_human_idle.h"
#include "player.h"
#include "player_command.h"
#include "player_state.h"
#include "PlayerSystem.h"

using namespace DirectX;

void PlayerState_Human_Walk::Enter(PlayerSystem& playerSystem)
{
	PlayerState_Human_Ground::Enter(playerSystem);
}

void PlayerState_Human_Walk::HandleInput(PlayerSystem& playerSystem)
{
	// 入力システム取得
	const InputSystem* inputSystem = playerSystem.m_InputSystem;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = playerSystem.m_StateMachine;

	// 移動コンポーネント取得
	PlayerMovement* movement = playerSystem.m_Movement;

	// 入力値取得
	float inputX = inputSystem->GetValue<PlayerCommand_MoveX>();
	float inputZ = inputSystem->GetValue<PlayerCommand_MoveZ>();

	// 歩行処理
	movement->Walk(inputX, inputZ);

	// 歩行入力がなくなったらアイドル状態へ遷移
	if(inputX == 0.0f &&
		inputZ == 0.0f)
	{
		stateMachine->ChangeState(PlayerStates::HumanIdle, playerSystem);
		return;
	}

	PlayerState_Human::HandleInput(playerSystem);
}

void PlayerState_Human_Walk::Update(double elapsedTime, PlayerSystem& playerSystem)
{
	PlayerState_Human_Ground::Update(elapsedTime, playerSystem);
}