#include "player_state_human_midair.h"
#include "player.h"
#include "PlayerSystem.h"

void PlayerState_Human_MidAir::Enter(PlayerSystem& playerSystem)
{
	PlayerState_Human::Enter(playerSystem);
}

void PlayerState_Human_MidAir::HandleInput(PlayerSystem& playerSystem)
{
	// 入力システム取得
	const InputSystem* inputSystem = playerSystem.m_InputSystem;

	// 移動コンポーネント取得
	PlayerMovement* movement = playerSystem.m_Movement;

	// 入力値取得
	float inputX = inputSystem->GetValue<PlayerCommand_MoveX>();
	float inputZ = inputSystem->GetValue<PlayerCommand_MoveZ>();

	// 電気ジャンプ処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
		movement->ElectricJump(inputX, inputZ);

	PlayerState_Human::HandleInput(playerSystem);
}

void PlayerState_Human_MidAir::Update(double elapsedTime, PlayerSystem& playerSystem)
{
	PlayerState_Human::Update(elapsedTime, playerSystem);
}