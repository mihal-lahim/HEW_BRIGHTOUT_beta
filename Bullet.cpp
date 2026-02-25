///////////////////////////////////////////
//’eŠÇ—‚ÆŽõ–½‚ð§Œä‚·‚éBulletƒNƒ‰ƒX‚ÌŽÀ‘•
///////////////////////////////////////////

#include "Bullet.h"
#include "Time.h"
#include "GameObject.h"

void Bullet::Update()
{
	float dt = (float)Time::DeltaTime();
	m_LifeTimer += dt;
	if (m_LifeTimer >= LifeTime)
	{
		gameObject().Destroy();
		return;
	}

	Vector3 dir = Direction.IsZero() ? Vector3(0.0f, 0.0f, 1.0f) : Direction.Normalize();
	Vector3 nextPos = gameObject().transform().position() + dir * (Speed * dt);

	for (auto* enemy : GetGameObjectsByTag("Enemy"))
	{
		if (!enemy || !enemy->IsActiveInHierarchy())
			continue;

		Vector3 diff = enemy->transform().position() - nextPos;
		if (diff.Length() <= HitRadius)
		{
			enemy->Destroy();
			gameObject().Destroy();
			return;
		}
	}

	gameObject().transform().position() = nextPos;
}
