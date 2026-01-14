#include "player_state_human_midair.h"
#include "player.h"

void PlayerState_Human_MidAir::Enter()
{
	PlayerState_Human::Enter();
}

void PlayerState_Human_MidAir::HandleInput()
{
	// 入力システム取得
	const InputSystem* inputSystem = GetOwner()->GetComponent<InputSystem>();

	// 移動コンポーネント取得
	PlayerMovement* movement = GetOwner()->GetComponent<PlayerMovement>();

	// 入力値取得
	float inputX = inputSystem->GetValue<PlayerCommand_MoveX>();
	float inputZ = inputSystem->GetValue<PlayerCommand_MoveZ>();

	// 電気ジャンプ処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
		movement->ElectricJump(inputX, inputZ, static_cast<Player*>(GetOwner()));

	PlayerState_Human::HandleInput();
}

void PlayerState_Human_MidAir::Update(double elapsedTime)
{
	// 移動コンポーネント取得
	PlayerMovement* movement = GetOwner()->GetComponent<PlayerMovement>();

	// 重力適用
	movement->ApplyGravity(elapsedTime);

	PlayerState_Human::Update(elapsedTime);
}