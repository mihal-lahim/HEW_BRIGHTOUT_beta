#ifndef BULLET_H
#define BULLET_H

#include "ScriptComponent.h"
#include "Vector3.h"
#include <string>

class Bullet : public ScriptComponent
{
public:
	float Speed = 20.0f;
	float LifeTime = 1.5f;
	float MaxDistance = 30.0f;
	float HitRadius = 2.0f;
	Vector3 Direction{ 0.0f, 0.0f, 1.0f };
	float EffectUVRotation = 0.0f;

	float TrailSpawnDistance = 0.45f;
	float TrailLifeTime = 0.14f;
	float TrailScale = 0.55f;
	std::wstring TrailTexturePath = L"texture/kaminari_B.png";
	int TrailSheetColumns = 7;
	int TrailSheetRows = 3;
	int TrailSheetFrameCount = 21;
	float TrailAnimationFps = 30.0f;
	bool TrailAnimationLoop = false;

	// ヒットSEのボリューム（0.0?1.0）
	float HitSEVolume = 0.5f;

	void Start() override;
	void Update() override;

private:
	float m_LifeTimer = 0.0f;
	Vector3 m_StartPos{};
	Vector3 m_LastTrailSpawnPos{};
	int m_HitSE = -1;
};

#endif

