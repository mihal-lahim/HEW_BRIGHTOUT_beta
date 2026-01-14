#include "player_state_human.h"
#include "player_state_electric.h"
#include "player.h"

void PlayerState_Human::Enter()
{
	// モデルを人間形態に設定
	// TODO: GetOwner()からモデル設定

	PlayerState::Enter();
}

void PlayerState_Human::HandleInput()
{
	// 入力システム取得
	const InputSystem* inputSystem = GetOwner()->GetComponent<InputSystem>();

	// 移動コンポーネント取得
	PlayerMovement* movement = GetOwner()->GetComponent<PlayerMovement>();

	// 変身システム取得
	PlayerMorphSystem* morphSystem = GetOwner()->GetComponent<PlayerMorphSystem>();

	// ステートマシン取得
	PlayerStateMachine* stateMachine = GetOwner()->GetComponent<PlayerStateMachine>();

	// 変身処理
	if (inputSystem->IsIssued<PlayerCommand_Morph>() && morphSystem->CanMorph())
	{
		// 最寄りの電線IDを取得
		PowerLineID nearestLine = morphSystem->GetNearestPowerLineID();

		// 電気形態へ変身
		movement->SnapToPowerLine(nearestLine, *static_cast<Player*>(GetOwner()));

		// ステート変更
		stateMachine->ChangeState(GetOwner()->GetComponent<PlayerState_Electric>());
		return;
	}

	PlayerState::HandleInput();
}

void PlayerState_Human::Update(double elapsedTime)
{
	PlayerState::Update(elapsedTime);
}