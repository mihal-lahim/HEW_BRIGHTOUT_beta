///////////////////////////////////////////
//’eŠÇ—‚ÆŽõ–½‚ð§Œä‚·‚éBulletƒNƒ‰ƒX‚ÌŽÀ‘•
///////////////////////////////////////////

#include "Bullet.h"
#include "Time.h"
#include "GameObject.h"
#include "ScoreData.h"

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
	Vector3 nextPos = curPos + dir * (Speed * dt);

	for (auto* enemy : GetGameObjectsByTag("Enemy"))
	{
		if (!enemy || !enemy->IsActiveInHierarchy())
			continue;

		Vector3 enemyPos = enemy->transform().position();

		// ü•ª curPos¨nextPos ã‚ÌÅ‹ß“_‚Å”»’èi‚·‚è”²‚¯–hŽ~j
		Vector3 seg = nextPos - curPos;
		float segLenSq = seg.Dot(seg);
		float t = 0.0f;
		if (segLenSq > 0.0f)
		{
			t = (enemyPos - curPos).Dot(seg) / segLenSq;
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;
		}
		Vector3 closest = curPos + seg * t;
		Vector3 toEnemy = enemyPos - closest;

		if (toEnemy.Length() <= HitRadius)
		{
			ScoreData::Instance().killedEnemies++;
			DestroyWithChildren(*enemy);
			DestroyWithChildren(gameObject());
			return;
		}
	}

	gameObject().transform().position() = nextPos;
}
