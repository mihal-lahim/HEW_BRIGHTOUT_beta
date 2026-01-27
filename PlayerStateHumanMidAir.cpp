
#include "Player.h"
#include "DebugCounter.h"

void PlayerState_Human_MidAir::Enter(Player& player)
{
	PlayerState_Human::Enter(player);
}

void PlayerState_Human_MidAir::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem* inputSystem = player.m_InputSystem;

	// 移動コンポーネント取得
	PlayerMovement* movement = player.m_Movement;

	// 入力値取得
	float inputX = inputSystem->GetValue<PlayerCommand_MoveX>();
	float inputZ = inputSystem->GetValue<PlayerCommand_MoveZ>();

	// 電気ジャンプ処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
		movement->ElectricJump(inputX, inputZ);


	PlayerState_Human::HandleInput(player);
}

void PlayerState_Human_MidAir::Update(Player& player)
{
	PlayerState_Human::Update(player);
}