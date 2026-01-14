#include "player_state_human_ground.h"
#include "player.h"
#include "PlayerSystem.h"

using namespace DirectX;

void PlayerState_Human_Ground::Enter(PlayerSystem& playerSystem)
{
	PlayerState_Human::Enter(playerSystem);
}

void PlayerState_Human_Ground::HandleInput(PlayerSystem& playerSystem)
{
	// 入力システム取得
	const InputSystem* inputSystem = playerSystem.m_InputSystem;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = playerSystem.m_StateMachine;

	// 移動コンポーネント取得
	PlayerMovement* movement = playerSystem.m_Movement;

	// ジャンプ処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
	{
		// 入力値取得
		float inputX = inputSystem->GetValue<PlayerCommand_MoveX>();
		float inputZ = inputSystem->GetValue<PlayerCommand_MoveZ>();

		movement->GroundJump(inputX, inputZ);
	}


	PlayerState_Human::HandleInput(playerSystem);
}

void PlayerState_Human_Ground::Update(double elapsedTime, PlayerSystem& playerSystem)
{
	// 移動コンポーネント取得
	PlayerMovement* movement = playerSystem.m_Movement;

	// 速度リセット
	movement->ResetVelocity();

	PlayerState_Human::Update(elapsedTime, playerSystem);
}