
#include "Player.h"
#include "GameTime.h"

using namespace DirectX;

void PlayerState_Human_Walk::Enter(Player& player)
{
	PlayerState_Human_Ground::Enter(player);
}

void PlayerState_Human_Walk::HandleInput(Player& player)
{
	// 入力システム取得
	InputHandler* inputHandler = player.inputHandler;

	// ステートマシン取得
	PlayerStateMachine* stateMachine = player.stateMachine;

	// 移動コンポーネント取得
	PlayerMovement* movement = player.movement;

	// 入力値取得
	float inputX = inputHandler->GetValue<PlayerCommand_MoveX>();
	float inputZ = inputHandler->GetValue<PlayerCommand_MoveZ>();

	// 歩行処理
	movement->Walk(inputX, inputZ);

	// 移動方向にプレイヤー本体の向きを回転
	// モデルは PlayerPrefab 側でローカル回転(180度)を与えているため、ここで重ねて回さない
	Vector3 moveDir = movement->MoveVec;
	if (!(moveDir.x == 0.0f && moveDir.z == 0.0f))
	{
		float yaw = XMConvertToDegrees(atan2f(moveDir.x, moveDir.z));
		player.SetFacingYaw(yaw);
		player.ApplyFacingYaw();
	}

	// 歩行入力がなくなったらアイドル状態へ遷移
	if(inputX == 0.0f &&
		inputZ == 0.0f)
	{
		stateMachine->ChangeState(&PlayerStates::HumanIdle, player);
		return;
	}

	PlayerState_Human_Ground::HandleInput(player);
}

void PlayerState_Human_Walk::Update(Player& player)
{
	PlayerState_Human_Ground::Update(player);
}