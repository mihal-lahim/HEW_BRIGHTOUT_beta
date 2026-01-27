#include "Player.h"
#include "debug_ostream.h"

#include <iostream>


void PlayerState_Human::Enter(Player& player)
{
	// 剛体を有効化
	player.m_RigidBody->SetActive(true);

	// モデルを人間形態に設定
	player.m_MeshRenderer->SetModel(player.m_HumanModel);

	PlayerState::Enter(player);
}

void PlayerState_Human::HandleInput(Player& player)
{
	hal::dout << "Human" << std::endl;

	// 入力システム取得
	const InputSystem* inputSystem = player.m_InputSystem;

	// 移動コンポーネント取得
	PlayerMovement* movement = player.m_Movement;

	// 変身システム取得
	PlayerMorphSystem* morphSystem = player.m_MorphSystem;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = player.m_StateMachine;

	// 変身処理
	if (inputSystem->IsIssued<PlayerCommand_Morph>() && morphSystem->CanMorph())
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
	PlayerMovement* movement = player.m_Movement;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = player.m_StateMachine;

	// 空中判定
	if (!movement->IsOnGround())
	{
		stateMachine->ChangeState(&PlayerStates::HumanMidAir, player);
		return;
	}

	PlayerState::Update(player);
}