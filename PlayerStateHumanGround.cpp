#include "PlayerState_Human_Ground.h"
#include "Player.h"
#include "PowerPlant.h"


using namespace DirectX;

void PlayerState_Human_Ground::Enter(Player& player)
{
	// 速度リセット
	player.movement->VelocityVec = { 0.0f, 0.0f, 0.0f };

	PlayerState_Human::Enter(player);
}

void PlayerState_Human_Ground::HandleInput(Player& player)
{

	// 入力システム取得
	InputHandler* inputHandler = player.inputHandler;

	// ジャンプ処理は無効化

	// インタラクト処理（発電所の復旧）
	if (inputHandler->IsIssued<PlayerCommand_Interact>())
	{
		Vector3 playerPos = player.gameObject().transform().position();
		auto powerPlants = player.GetGameObjectsByTag("PowerPlant");
		for (auto* obj : powerPlants)
		{
			PowerPlant* plant = obj->GetComponent<PowerPlant>();
			if (plant && !plant->IsRestored() && plant->IsInRange(playerPos))
			{
				plant->Restore();
				break;
			}
		}
	}

	PlayerState_Human::HandleInput(player);
}

void PlayerState_Human_Ground::Update(Player& player)
{
	PlayerState_Human::Update(player);
}