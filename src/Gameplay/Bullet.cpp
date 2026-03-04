///////////////////////////////////////////
//弾管理と自動消滅を制御するBulletクラスの実装
///////////////////////////////////////////

#include "Bullet.h"
#include "GameTime.h"
#include "GameObject.h"
#include "ScoreData.h"
#include "Enemy.h"

#include "Ray.h"
#include "PhysicsSystem.h"
#include "PhysicsBody.h"
#include "Health.h"

#include "audio.h"


namespace
{
	void DestroyWithChildren(GameObject& obj)
	{
		// 既に破棄済みならスキップ
		if (!obj.IsActiveSelf()) return;

		for (auto* child : obj.GetChildren())
		{
			if (child) DestroyWithChildren(*child);
		}
		obj.SetActive(false);
		obj.Destroy();
	}
}

void Bullet::Start()
{
	m_StartPos = gameObject().transform().position();
	m_HitSE = LoadAudio("sound/bullet_hit.wav");
}

void Bullet::Update()
{
	float dt = (float)Time::DeltaTime();
	m_LifeTimer += dt;
	if (m_LifeTimer >= LifeTime)
	{
		DestroyWithChildren(gameObject());
		return;
	}

	Vector3 diff = gameObject().transform().position() - m_StartPos;
	if (diff.Length() >= MaxDistance)
	{
		DestroyWithChildren(gameObject());
		return;
	}

	Vector3 dir = Direction.IsZero() ? Vector3(0.0f, 0.0f, 1.0f) : Direction.Normalize();
	Vector3 curPos = gameObject().transform().position();
	float stepDist = Speed * dt + HitRadius;

	// RayCastで進行方向のヒット判定（BVH最適化済み）
	Ray ray(curPos, dir);
	physics().RayCast(ray, stepDist);

	if (ray.IsHit && ray.HitObject)
	{
		// ヒットしたPhysicsBodyのGameObjectが有効か確認
		auto& hitGameObject = ray.HitObject->gameObject();
		if (hitGameObject.CompareTag("Enemy") && hitGameObject.IsActiveInHierarchy())
		{
			auto* enemyComponent = hitGameObject.GetComponent<Enemy>();

			// 既に倒された敵はスキップして弾の移動を継続する
			if (enemyComponent && enemyComponent->IsDefeated())
			{
				// return せず下のposition更新へ進む
			}
			else if (enemyComponent)
			{
				// ヒットSE再生
				if (m_HitSE >= 0)
				{
					PlayAudio(m_HitSE, false);
					SetAudioVolume(m_HitSE, HitSEVolume);
				}

				ScoreData::Instance().killedEnemies++;
				enemyComponent->OnDefeated();

				// プレイヤーのHPを30回復
				GameObject* player = GetGameObjectByTag("Player");
				if (player)
				{
					auto* playerHealth = player->GetComponent<Health>();
					if (playerHealth)
					{
						playerHealth->Heal(30.0f);
					}
				}

				DestroyWithChildren(gameObject());
				return;
			}
			else
			{
				// Enemyコンポーネントがないが"Enemy"タグがついている場合
				// ヒットSE再生
				if (m_HitSE >= 0)
				{
					PlayAudio(m_HitSE, false);
					SetAudioVolume(m_HitSE, HitSEVolume);
				}

				DestroyWithChildren(gameObject());
				return;
			}
		}
	}

	Vector3 nextPos = curPos + dir * (Speed * dt);
	gameObject().transform().position() = nextPos;
}
