#include "PowerPlant.h"
#include "GameObject.h"
#include "PowerPlantUI.h"
#include "Health.h"
#include <algorithm>
#include <cstdlib>
#include "Player.h"
#include "ScoreData.h"
#include "Audio.h" 

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

// 全 PowerPlant で共有するホールドSE
static int s_holdSE = -1;
static bool s_holdSELoaded = false;

void PowerPlant::Start()
{
	// コンポーネント初期化後に復旧モデルを非表示にする
	SetActiveRecursive(restoredModel, false);
	m_holdTimer = 0.0f;
	m_wasHolding = false;

	// PowerPlantUI を追加
	gameObject().AddComponent<PowerPlantUI>();

	// SE を一度だけロード（
	if (!s_holdSELoaded)
	{
		s_holdSE = LoadAudio("sound/gauge_restore.wav"); 
		if (s_holdSE >= 0)
		{
			SetAudioVolume(s_holdSE, 0.7f); 
		}
		s_holdSELoaded = true;
	}
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


				// 復旧したらホールドSEは停止
				if (s_holdSELoaded && s_holdSE >= 0)
				{
					StopAudio(s_holdSE);
				}


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

		// 長押しが開始された瞬間にループSEを再生
		if (!m_wasHolding)
		{
			if (s_holdSELoaded && s_holdSE >= 0)
			{
				PlayAudio(s_holdSE, true); // ループ再生
			}
		}

		m_holdTimer += deltaTime;
		m_wasHolding = true;


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
			// 復旧で SE を停止（
			if (s_holdSELoaded && s_holdSE >= 0)
			{
				StopAudio(s_holdSE);
			}
			m_holdTimer = m_holdThreshold;
			m_wasHolding = false;
		}
	}
	else
	{
		// 長押しが解除された瞬間にループSEを停止
		if (m_wasHolding)
		{
			if (s_holdSELoaded && s_holdSE >= 0)
			{
				StopAudio(s_holdSE);
			}
			m_wasHolding = false;
		}
	}
}

void PowerPlant::ResetHold()
{
	m_holdTimer = 0.0f;

	// ホールドがリセットされたら SE を停止
	if (m_wasHolding)
	{
		if (s_holdSELoaded && s_holdSE >= 0)
		{
			StopAudio(s_holdSE);
		}
		m_wasHolding = false;
	}
}

bool PowerPlant::IsInRange(const Vector3& position) const
{
	Vector3 plantPos = gameObject().transform().position();
	Vector3 diff = position - plantPos;
	float distSq = diff.x * diff.x + diff.z * diff.z;
	return distSq <= m_interactRadius * m_interactRadius;
}

