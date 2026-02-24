#ifndef BULLET_H
#define BULLET_H

#include "ScriptComponent.h"
#include "Vector3.h"

class Bullet : public ScriptComponent
{
public:
	float Speed = 20.0f;
	float LifeTime = 3.0f;
	float HitRadius = 0.5f;
	Vector3 Direction{ 0.0f, 0.0f, 1.0f };

	void Update() override;

private:
	float m_LifeTimer = 0.0f;
};

#endif
