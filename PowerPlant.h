#ifndef POWER_PLANT_H
#define POWER_PLANT_H

#include "ScriptComponent.h"
#include "Vector3.h"
#include <algorithm>


class GameObject;

class PowerPlant : public ScriptComponent
{
public:
	PowerPlant(float interactRadius = 5.0f)
		: m_interactRadius(interactRadius)
	{}
	virtual ~PowerPlant() = default;

	// コンポーネント初期化後に呼ばれる
	void Start() override;

	// 復旧済みか
	bool IsRestored() const { return m_isRestored; }

	// 直ちに復旧処理を行う
	void Restore();

	// 長押し状態を更新する（deltaTimeは前フレームからの経過時間、isHoldingは現在押されているか）
	void UpdateHold(float deltaTime, bool isHolding);

	// 長押しタイマーをリセットする
	void ResetHold();

	// プレイヤーの位置から範囲内か
	bool IsInRange(const Vector3& position) const;

	// インタラクト半径
	float GetInteractRadius() const { return m_interactRadius; }

	// モデル参照
	GameObject* restoredModel = nullptr;
	GameObject* brokenModel = nullptr;

	// 長押し進捗を 0..1 で取得（UI 用）
	float GetHoldProgress() const
	{
		if (m_holdThreshold <= 0.0f) return 0.0f;
		return std::clamp(m_holdTimer / m_holdThreshold, 0.0f, 1.0f);
	}

private:
	bool m_isRestored = false;
	float m_interactRadius = 5.0f;

	// 長押し判定用
	float m_holdTimer = 0.0f;
	float m_holdThreshold = 1.5f; // 長押し秒数
};

#endif
