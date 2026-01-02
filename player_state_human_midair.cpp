#include "player_state_human_midair.h"
#include "player.h"

void PlayerState_Human_MidAir::Enter(Player& player)
{
	PlayerState_Human::Enter(player);
}

void PlayerState_Human_MidAir::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem& inputSystem = player.GetInputSystem();

	// 移動コンポーネント取得
	PlayerMovement* movement = player.GetMovement();

	// 入力値取得
	float inputX = inputSystem.GetCommandValue<PlayerCommand_WalkX>();
	float inputZ = inputSystem.GetCommandValue<PlayerCommand_WalkZ>();

	// 電気ジャンプ処理
	if (inputSystem.IsCommandIssued<PlayerCommand_ElectricJump>())
		movement->ElectricJump(inputX, inputZ, &player);

	PlayerState_Human::HandleInput(player);
}

void PlayerState_Human_MidAir::Update(Player& player, double elapsedTime)
{
	// 重力適用
	player.GetMovement()->ApplyGravity(elapsedTime);

	PlayerState_Human::Update(player, elapsedTime);
}

void PlayerState_Human_MidAir::Draw(const Player& player) const
{
	PlayerState_Human::Draw(player);
}

void PlayerState_Human_MidAir::Exit(Player& player)
{
	PlayerState_Human::Exit(player);
}
