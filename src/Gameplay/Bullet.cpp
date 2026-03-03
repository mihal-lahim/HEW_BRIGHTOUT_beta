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

#include "audio.h"


namespace
{
	void DestroyWithChildren(GameObject& obj)
	{
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
		auto& hitGameObject = ray.HitObject->gameObject();
		if (hitGameObject.CompareTag("Enemy") && hitGameObject.IsActiveInHierarchy())
		{


			auto* enemyComponent = enemy->GetComponent<Enemy>();
			if (enemyComponent && enemyComponent->IsDefeated())
			{
				continue;
			}

			// ヒットSE再生
			if (m_HitSE >= 0)
			{
				PlayAudio(m_HitSE, false);
				SetAudioVolume(m_HitSE, HitSEVolume);
			}

			ScoreData::Instance().killedEnemies++;
			if (enemyComponent)

			{
				ScoreData::Instance().killedEnemies++;
				enemyComponent->OnDefeated();
				DestroyWithChildren(gameObject());
				return;
			}
		}
	}

	Vector3 nextPos = curPos + dir * (Speed * dt);
	gameObject().transform().position() = nextPos;
}
