#include "player_state_human.h"
#include "player.h"

void PlayerState_Human::Enter(Player&)
{
}

void PlayerState_Human::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem& inputSystem = player.GetInputSystem();

	// 移動コンポーネント取得
	PlayerMovement* movement = player.GetMovement();

	// 変身処理
	if (inputSystem.IsCommandIssued<PlayerCommand_Morph>())
	{}
}

void PlayerState_Human::Update(Player& player, double elapsedTime)
{
	// 移動コンポーネントの更新処理
	player.GetMovement()->Update(elapsedTime, &player);
}

void PlayerState_Human::Draw(const Player&) const
{
}

void PlayerState_Human::Exit(Player&)
{
}
