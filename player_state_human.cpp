#include "player_state_human.h"
#include "player.h"

void PlayerState_Human::Enter(Player& player)
{
	// モデルを人間形態に設定
	player.SetModel(player.GetHumanModel());

	PlayerState::Enter(player);
}

void PlayerState_Human::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem* inputSystem = player.GetInputSystem();

	// 移動コンポーネント取得
	PlayerMovement* movement = player.GetMovement();

	// 変身処理
	if (inputSystem->IsIssued<PlayerCommand_Morph>() && player.GetMorphSystem()->CanMorph(player))
	{
		// 最寄りの電線IDを取得
		PowerLineID nearestLine = player.GetMorphSystem()->GetNearestPowerLineID(player);

		// 電気形態へ変身
		movement->SnapToPowerLine(nearestLine, player);

		// ステート変更
		player.ChangeState(&PlayerStates::Electric);
		return;
	}

	PlayerState::HandleInput(player);
}

void PlayerState_Human::Update(Player& player, double elapsedTime)
{
	PlayerState::Update(player, elapsedTime);
}

void PlayerState_Human::Draw(const Player& player) const
{
	PlayerState::Draw(player);
}
