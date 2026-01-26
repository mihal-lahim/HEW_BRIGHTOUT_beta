#include "PlayerState_Human_Ground.h"
#include "Player.h"

using namespace DirectX;

void PlayerState_Human_Ground::Enter(Player& player)
{
	PlayerState_Human::Enter(player);
}

void PlayerState_Human_Ground::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem* inputSystem = player.m_InputSystem;

	// 移動コンポーネント取得
	PlayerMovement* movement = player.m_Movement;

	// ジャンプ処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
	{
		// 入力値取得
		float inputX = inputSystem->GetValue<PlayerCommand_MoveX>();
		float inputZ = inputSystem->GetValue<PlayerCommand_MoveZ>();

		movement->GroundJump(inputX, inputZ);
	}


	PlayerState_Human::HandleInput(player);
}

void PlayerState_Human_Ground::Update(Player& player)
{
	// 移動コンポーネント取得
	PlayerMovement* movement = player.m_Movement;

	// 速度リセット
	movement->ResetVelocity();

	PlayerState_Human::Update(player);
}