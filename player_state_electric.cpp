#include "player_state_electric.h"
#include "player_state_human_midair.h"
#include "player.h"

void PlayerState_Electric::Enter()
{
	// モデルを電気形態に設定
	// TODO: GetOwner()からモデル設定

	// 移動コンポーネント取得
	PlayerMovement* movement = GetOwner()->GetComponent<PlayerMovement>();

	// 変身コンポーネント取得
	PlayerMorphSystem* morphSystem = GetOwner()->GetComponent<PlayerMorphSystem>();

	// 電線上にスナップ
	movement->SnapToPowerLine(morphSystem->GetNearestPowerLineID(), *static_cast<Player*>(GetOwner()));

	PlayerState::Enter();
}

void PlayerState_Electric::HandleInput()
{
	// 入力システム取得
	const InputSystem* inputSystem = GetOwner()->GetComponent<InputSystem>();

	// 移動コンポーネント取得
	PlayerMovement* movement = GetOwner()->GetComponent<PlayerMovement>();

	// ステートマシン取得
	PlayerStateMachine* stateMachine = GetOwner()->GetComponent<PlayerStateMachine>();

	// ジャンプコマンドが発行されたら射出・変身処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
	{
		// 電線から射出
		movement->Eject(
			inputSystem->GetValue<PlayerCommand_MoveX>(),
			inputSystem->GetValue<PlayerCommand_MoveZ>(),
			static_cast<Player*>(GetOwner()));

		// 人間形態へ変身
		stateMachine->ChangeState(GetOwner()->GetComponent<PlayerState_Human_MidAir>());
		return;
	}

	// 移動コマンドが発行されたら方向転換処理
	if (inputSystem->IsIssued<PlayerCommand_MoveX>()
		|| inputSystem->IsIssued<PlayerCommand_MoveZ>())
	{
		// 電線上方向指定処理
		movement->Turn(
			inputSystem->GetValue<PlayerCommand_MoveX>(),
			inputSystem->GetValue<PlayerCommand_MoveZ>(),
			static_cast<Player*>(GetOwner()));
	}

	PlayerState::HandleInput();
}

void PlayerState_Electric::Update(double elapsedTime)
{
	// 移動コンポーネント取得
	PlayerMovement* movement = GetOwner()->GetComponent<PlayerMovement>();

	// 電線上移動処理
	movement->Move(static_cast<Player*>(GetOwner()));

	PlayerState::Update(elapsedTime);
}
