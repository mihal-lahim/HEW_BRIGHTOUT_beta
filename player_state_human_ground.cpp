
#include "player_state_human_ground.h"
#include "player.h"

using namespace DirectX;

void PlayerState_Human_Ground::Enter(Player& player)
{
	// 地上状態に入る際に速度ベクトルをリセット
	player.GetMovement()->SetVelocityVec(DirectX::XMVectorZero());

	PlayerState_Human::Enter(player);
}

void PlayerState_Human_Ground::HandleInput(Player& player)
{
	// 入力システム取得
	const InputSystem* inputSystem = player.GetInputSystem();

	// 移動コンポーネント取得
	PlayerMovement* movement = player.GetMovement();

	// 入力値取得
	float inputX = inputSystem->GetValue<PlayerCommand_MoveX>();
	float inputZ = inputSystem->GetValue<PlayerCommand_MoveZ>();

	// 歩行処理
	movement->Walk(inputX, inputZ, &player);

	// 通常ジャンプ処理
	if (inputSystem->IsIssued<PlayerCommand_Jump>())
		movement->GroundJump(inputX, inputZ, &player);

	PlayerState_Human::HandleInput(player);
}

void PlayerState_Human_Ground::Update(Player& player, double elapsedTime)
{
	PlayerState_Human::Update(player, elapsedTime);
}

void PlayerState_Human_Ground::Draw(const Player& player) const
{
	PlayerState_Human::Draw(player);
}