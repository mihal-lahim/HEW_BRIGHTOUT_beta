
#include "Player.h"
#include "DebugCounter.h"
#include "debug_ostream.h"

#include <iostream>

using namespace DirectX;

void PlayerState_Human_Walk::Enter(Player& player)
{
	PlayerState_Human_Ground::Enter(player);
}

void PlayerState_Human_Walk::HandleInput(Player& player)
{
	hal::dout << "HumanWalk" << std::endl;

	// 入力システム取得
	const InputSystem* inputSystem = player.m_InputSystem;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = player.m_StateMachine;

	// 移動コンポーネント取得
	PlayerMovement* movement = player.m_Movement;

	// 入力値取得
	float inputX = inputSystem->GetValue<PlayerCommand_MoveX>();
	float inputZ = inputSystem->GetValue<PlayerCommand_MoveZ>();

	// 歩行処理
	movement->Walk(inputX, inputZ);

	// 歩行入力がなくなったらアイドル状態へ遷移
	if(inputX == 0.0f &&
		inputZ == 0.0f)
	{
		stateMachine->ChangeState(&PlayerStates::HumanIdle, player);
		return;
	}

	PlayerState_Human_Ground::HandleInput(player);
}

void PlayerState_Human_Walk::Update(Player& player)
{
	PlayerState_Human_Ground::Update(player);
}