//復旧システム

#include "ElectricRestoreSystem.h"
#include "Player.h"

ElectricRestoreSystem::ElectricRestoreSystem(
	const DirectX::XMFLOAT3& pos,
	float durability)
	: GameObject(pos)
	, m_Durability(durability)
	, m_MaxDurability(durability)
{
}

void ElectricRestoreSystem::Interact(Player* player)
{
	//すでに復旧中の場合何もしない
	if (m_IsRestoring) return;

	// 電気量が足りない場合何もしない
	if (player->GetElectricityRate() < START_COST_RATE)
	{
		return;
	}

	// 40%支払う
	player->ConsumeElectricityRate(START_COST_RATE);

	// 復旧開始
	m_IsRestoring = true;
}

void ElectricRestoreSystem::TakeDamage(float damage)
{
	if (!m_IsRestoring) return;

	m_Durability -= damage;

	if (m_Durability <= 0.0f)
	{
		m_Durability = 0.0f;
		m_IsRestoring = false; // 復旧中断
	}
}

void ElectricRestoreSystem::Update(double elapsedTime)
{
	if (!m_IsRestoring) return;
}

void ElectricRestoreSystem::Draw() const
{
	
}