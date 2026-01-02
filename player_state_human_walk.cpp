
#include "player.h"
#include "player_command.h"
#include "player_state.h"

using namespace DirectX;

void PlayerState_Human_Walk::Enter(Player& player)
{
	PlayerState_Human_Ground::Enter(player);
}

void PlayerState_Human_Walk::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem& inputSystem = player.GetInputSystem();

	// 歩行入力がなくなったらアイドル状態へ遷移
	if(inputSystem.IsCommandIssued<PlayerCommand_WalkX>() == 0.0f &&
		inputSystem.IsCommandIssued<PlayerCommand_WalkZ>() == 0.0f)
	{
		player.ChangeState(&PlayerStates::HumanIdle);
		return;
	}

	PlayerState_Human::HandleInput(player);
}

void PlayerState_Human_Walk::Update(Player& player, double elapsedTime)
{
	PlayerState_Human_Ground::Update(player, elapsedTime);
}

void PlayerState_Human_Walk::Draw(const Player& player) const
{
	PlayerState_Human_Ground::Draw(player);
}

void PlayerState_Human_Walk::Exit(Player& player)
{
	PlayerState_Human_Ground::Exit(player);
}
