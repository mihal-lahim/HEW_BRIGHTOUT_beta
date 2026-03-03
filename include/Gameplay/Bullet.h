#ifndef BULLET_H
#define BULLET_H

#include "ScriptComponent.h"
#include "Vector3.h"

class Bullet : public ScriptComponent
{
public:
	float Speed = 20.0f;
	float LifeTime = 1.5f;
	float MaxDistance = 30.0f;
	float HitRadius = 2.0f;
	Vector3 Direction{ 0.0f, 0.0f, 1.0f };

	// ヒットSEのボリューム（0.0?1.0）
	float HitSEVolume = 0.5f;

	void Start() override;
	void Update() override;

private:
	float m_LifeTimer = 0.0f;
	Vector3 m_StartPos{};
	int m_HitSE = -1;
};

#endif
