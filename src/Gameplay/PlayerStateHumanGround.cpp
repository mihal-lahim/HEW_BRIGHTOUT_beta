#include "PlayerState_Human_Ground.h"
#include "Player.h"
#include "PowerPlant.h"
#include "GameTime.h"


using namespace DirectX;

void PlayerState_Human_Ground::Enter(Player& player)
{
	// 速度リセット
	player.movement->VelocityVec = { 0.0f, 0.0f, 0.0f };

	// PowerPlantのキャッシュをリセットして再取得
	m_powerPlantsCached = false;

	PlayerState_Human::Enter(player);
}

void PlayerState_Human_Ground::HandleInput(Player& player)
{

	// 入力システム取得
	InputHandler* inputHandler = player.inputHandler;

	// インタラクト入力（発電所の復旧 ? 復旧ゲージ加算）
	bool isHolding = inputHandler->IsIssued<PlayerCommand_Interact>();
	float deltaTime = static_cast<float>(Time::DeltaTime());
	Vector3 playerPos = player.gameObject().transform().position();

	// PowerPlantリストを初回のみキャッシュ
	if (!m_powerPlantsCached)
	{
		m_cachedPowerPlants = player.GetGameObjectsByTag("PowerPlant");
		m_powerPlantsCached = true;
	}

	for (auto* obj : m_cachedPowerPlants)
	{
		if (!obj) continue;
		PowerPlant* plant = obj->GetComponent<PowerPlant>();
		if (!plant || plant->IsRestored())
			continue;

		if (plant->IsInRange(playerPos))
		{
			// 範囲内: Bボタンの押し下ろ状態に応じてホールドタイマーを更新
			plant->UpdateHold(deltaTime, isHolding);
		}
		else
		{
			// 範囲外: ホールドタイマーをリセット
			plant->ResetHold();
		}
	}

	PlayerState_Human::HandleInput(player);
}

void PlayerState_Human_Ground::Update(Player& player)
{
	PlayerState_Human::Update(player);
}