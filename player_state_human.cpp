
#include "player.h"


void PlayerState_Human::Enter(PlayerSystem& playerSystem)
{
	// モデルを人間形態に設定
	playerSystem.m_MeshRenderer->SetModel(playerSystem.m_HumanModel);

	PlayerState::Enter(playerSystem);
}

void PlayerState_Human::HandleInput(PlayerSystem& playerSystem)
{
	// 入力システム取得
	const InputSystem* inputSystem = playerSystem.m_InputSystem;

	// 移動コンポーネント取得
	PlayerMovement* movement = playerSystem.m_Movement;

	// 変身システム取得
	PlayerMorphSystem* morphSystem = playerSystem.m_MorphSystem;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = playerSystem.m_StateMachine;

	// 変身処理
	if (inputSystem->IsIssued<PlayerCommand_Morph>() && morphSystem->CanMorph())
	{
		// 最寄りの電線IDを取得
		PowerLineID nearestLine = morphSystem->GetNearestPowerLineID();

		// 電気形態へ変身
		movement->SnapToPowerLine(nearestLine);

		// ステート変更
		stateMachine->ChangeState(PlayerStates::Electric, playerSystem);
		return;
	}


	PlayerState::HandleInput(playerSystem);
}

void PlayerState_Human::Update(double elapsedTime, PlayerSystem& playerSystem)
{
	// 移動コンポーネント取得
	PlayerMovement* movement = playerSystem.m_Movement;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = playerSystem.m_StateMachine;

	// 空中判定
	if (!movement->IsOnGround())
	{
		stateMachine->ChangeState(PlayerStates::HumanMidAir, playerSystem);
		return;
	}

	PlayerState::Update(elapsedTime, playerSystem);
}