

#ifndef POWER_PLANT_H
#define POWER_PLANT_H

#include "ScriptComponent.h"
#include "Vector3.h"

class GameObject;

class PowerPlant : public ScriptComponent
{
public:
	PowerPlant(float interactRadius = 5.0f)
		: m_interactRadius(interactRadius)
	{}
	virtual ~PowerPlant() = default;

	// 初期化（復旧モデルを非表示にする）
	void Start() override;

	// 復旧状態かどうか
	bool IsRestored() const { return m_isRestored; }

	// 復旧処理（モデル表示を切り替える）
	void Restore();

	// 指定座標が範囲内かどうか
	bool IsInRange(const Vector3& position) const;

	// インタラクト可能な範囲
	float GetInteractRadius() const { return m_interactRadius; }

	// モデルオブジェクトの設定
	GameObject* restoredModel = nullptr;
	GameObject* brokenModel = nullptr;

private:
	bool m_isRestored = false;
	float m_interactRadius = 5.0f;
};

#endif
