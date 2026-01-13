#ifndef ELECTRIC_RESTORE_SYSTEM_H
#define ELECTRIC_RESTORE_SYSTEM_H

#include "GameObject.h"

class Player;

// 電気復旧システム
class ElectricRestoreSystem : public GameObject
{
public:
	// 定数
	static constexpr float START_COST_RATE = 0.4f; // 40%

private:
	bool  m_IsRestoring = false; // 復旧中か
	float m_Durability = 100.0f; // 耐久値
	float m_MaxDurability = 100.0f;

public:
	ElectricRestoreSystem(
		const DirectX::XMFLOAT3& pos,
		float durability = 100.0f);

	// インタラクト（プレイヤーから呼ぶ）
	void Interact(Player* player);

	// ダメージを受ける
	void TakeDamage(float damage);

	// 状態取得
	bool IsRestoring() const { return m_IsRestoring; }
	float GetDurabilityRate() const { return m_Durability / m_MaxDurability; }

	virtual void Update(double elapsedTime) override;
	virtual void Draw() const override;
};

#endif // ELECTRIC_RESTORE_SYSTEM_H