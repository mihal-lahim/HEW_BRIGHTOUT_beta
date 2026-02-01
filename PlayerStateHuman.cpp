#include "Player.h"


void PlayerState_Human::Enter(Player& player)
{
	// 剛体を有効化
	player.physicsBody->SetEnable(true);

	// モデルを人間形態に設定
	player.meshRenderer->SetModel(player.humanModel);

	PlayerState::Enter(player);
}

void PlayerState_Human::HandleInput(Player& player)
{
	//hal::dout << "Human" << std::endl;

	// 入力システム取得
	InputHandler* inputHandler = player.inputHandler;

	// 移動コンポーネント取得
	PlayerMovement* movement = player.movement;

	// 変身システム取得
	PlayerMorphSystem* morphSystem = player.morphSystem;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = player.stateMachine;

	// 変身処理
	if (inputHandler->IsIssued<PlayerCommand_Morph>() && morphSystem->CanMorph())
	{
		// 最寄りの電線IDを取得
		PowerLineID nearestLine = morphSystem->GetNearestPowerLineID();

		// 電気形態へ変身
		movement->SnapToPowerLine(nearestLine);

		// ステート変更
		stateMachine->ChangeState(&PlayerStates::Electric, player);
		return;
	}

	PlayerState::HandleInput(player);
}

void PlayerState_Human::Update(Player& player)
{
	// 移動コンポーネント取得
	PlayerMovement* movement = player.movement;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = player.stateMachine;

	// 空中判定
	if (!movement->IsOnGround())
	{
		stateMachine->ChangeState(&PlayerStates::HumanMidAir, player);
		return;
	}

	PlayerState::Update(player);
}