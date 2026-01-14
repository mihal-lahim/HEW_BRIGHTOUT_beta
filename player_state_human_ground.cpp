#include "player_state_human_ground.h"
#include "player.h"

using namespace DirectX;

void PlayerState_Human_Ground::Enter()
{
	// 移動コンポーネント取得
	PlayerMovement* movement = GetOwner()->GetComponent<PlayerMovement>();

	// 地上状態に入る際に速度ベクトルをリセット
	movement->SetVelocityVec(DirectX::XMVectorZero());

	PlayerState_Human::Enter();
}

void PlayerState_Human_Ground::HandleInput()
{
	// 入力システム取得
	const InputSystem* inputSystem = GetOwner()->GetComponent<InputSystem>();

	// 移動コンポーネント取得
	PlayerMovement* movement = GetOwner()->GetComponent<PlayerMovement>();

	// 入力値取得
	float inputX = inputSystem->GetValue<PlayerCommand_MoveX>();
	float inputZ = inputSystem->GetValue<PlayerCommand_MoveZ>();

	// 歩行処理
	movement->Walk(inputX, inputZ, static_cast<Player*>(GetOwner()));

	// 通常ジャンプ処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
		movement->GroundJump(inputX, inputZ, static_cast<Player*>(GetOwner()));

	PlayerState_Human::HandleInput();
}

void PlayerState_Human_Ground::Update(double elapsedTime)
{
	PlayerState_Human::Update(elapsedTime);
}