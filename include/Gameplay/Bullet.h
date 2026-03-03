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

	std::wstring TexturePath = L"texture/ball.png";
	Vector3 VisualScale{ 0.8f, 0.8f, 0.8f };
	float VisualColorScale = 2.5f;
	float VisualAlpha = 1.0f;

	int TrailSheetColumns = 6;
	int TrailSheetRows = 6;
	int TrailSheetFrameCount = 31;
	float TrailAnimationFps = 12.5f;
	bool TrailAnimationLoop = true;

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

