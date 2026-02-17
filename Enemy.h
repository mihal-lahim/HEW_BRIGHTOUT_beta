#ifndef ENEMY_H
#define ENEMY_H

#include "Movement.h"

class Enemy : public Movement
{
public:
	float MoveSpeed = 5.0f;

	void Start() override
	{
		Movement::Start();
		m_target = GetGameObjectByTag("Player");
	}

	void Update() override
	{
		if (!m_target)
		{
			m_target = GetGameObjectByTag("Player");
			if (!m_target)
			{
				return;
			}
		}

		Vector3 direction = m_target->transform().position() - gameObject().transform().position();
		direction.y = 0.0f;

		float length = direction.Length();
		if (length <= 0.1f)
		{
			MoveVec = { 0.0f, 0.0f, 0.0f };
			return;
		}

		MoveVec = direction.Normalize() * MoveSpeed;
		RotateByMoveVec();
	}

private:
	GameObject* m_target = nullptr;
};

#endif
