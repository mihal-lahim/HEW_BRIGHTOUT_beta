#include "PowerPlant.h"
#include "GameObject.h"
#include "PowerPlantUI.h"
#include "Health.h"
#include <algorithm>
#include <cstdlib>
#include "Player.h"
#include "ScoreData.h"

 // GameObjectとその全子孫を再帰的にSetActiveする
static void SetActiveRecursive(GameObject* obj, bool active)
{
	if (!obj) return;
	obj->SetActive(active);
	for (auto* child : obj->GetChildren())
	{
		SetActiveRecursive(child, active);
	}
}

void PowerPlant::Start()
{
	// コンポーネント初期化後に復旧モデルを非表示にする
	SetActiveRecursive(restoredModel, false);
	m_holdTimer = 0.0f;

	// 同じ GameObject に UI コンポーネントを追加して表示させる
	// PowerPlantUI は Start() で自身の子として背景とフィルを生成します
	gameObject().AddComponent<PowerPlantUI>();
}

void PowerPlant::Restore()
{
	if (m_isRestored)
		return;

	// プレイヤーが復旧させる際の消費量（電気量）を固定
	const float cost = 5.0f;

	// プレイヤーの Health を探して消費する
	auto scene = gameObject().scenePtr();
	if (scene)
	{
		auto healths = scene->GetComponents<Health>();
		if (!healths.empty())
		{
			Health* playerHealth = nullptr;
			Player* targetPlayer = nullptr;
			for (auto* h : healths)
			{
				if (!h) continue;
				Player* player = h->gameObject().GetComponent<Player>();
				if (player != nullptr)
				{
					playerHealth = h;
					targetPlayer = player;
					break;
				}
			}
			if (!playerHealth) playerHealth = healths.front();

			if (playerHealth)
			{
				float cur = playerHealth->GetCurrentHealth();
				// HP が cost 未満なら復旧不可（消費もしない）
				if (cur < cost)
				{
					return;
				}

				// 必要分を消費して復旧
				playerHealth->SetCurrentHealth(cur - cost);

				// 発電機を復旧状態にし、モデル切替
				m_isRestored = true;
				ScoreData::Instance().restoredPlants++;
				SetActiveRecursive(brokenModel, false);
				SetActiveRecursive(restoredModel, true);
				if (targetPlayer)
				{
					if ((std::rand() % 2) == 0)
					{
						targetPlayer->ApplyPermanentMoveSpeedBuff();
					}
					else
					{
						targetPlayer->ApplyPermanentRestoreSpeedBuff();
					}
				}
				return;
			}
		}
	}

	// Health が見つからなければ何もしない
}

void PowerPlant::UpdateHold(float deltaTime, bool isHolding)
{
	if (m_isRestored)
		return;

	if (isHolding)
	{
		float restoreSpeedMultiplier = 1.0f;
		auto scene = gameObject().scenePtr();
		if (scene)
		{
			auto healths = scene->GetComponents<Health>();
			for (auto* h : healths)
			{
				if (!h) continue;
				Player* player = h->gameObject().GetComponent<Player>();
				if (player)
				{
					restoreSpeedMultiplier = player->GetRestoreSpeedMultiplier();
					break;
				}
			}
		}

		m_holdTimer += deltaTime * restoreSpeedMultiplier;
		if (m_holdTimer >= m_holdThreshold)
		{
			Restore();
			// タイマーは閾値で止めておく（必要なら続けて増やしてもよい）
			m_holdTimer = m_holdThreshold;
		}
	}
	// ボタンを離してもタイマーは保持（一時停止）
	// 範囲外に出た場合のみ ResetHold() でリセットされる
}

void PowerPlant::ResetHold()
{
	m_holdTimer = 0.0f;
}

bool PowerPlant::IsInRange(const Vector3& position) const
{
	Vector3 plantPos = gameObject().transform().position();
	Vector3 diff = position - plantPos;
	float distSq = diff.x * diff.x + diff.z * diff.z;
	return distSq <= m_interactRadius * m_interactRadius;
}

