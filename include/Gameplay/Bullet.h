#ifndef BULLET_H
#define BULLET_H

#include "ScriptComponent.h"
#include "Vector3.h"

class GameObject;

class Bullet : public ScriptComponent
{
public:
	float Speed = 20.0f;
	float LifeTime = 1.5f;
	float MaxDistance = 30.0f;
	float HitRadius = 15.0f;
	Vector3 Direction{ 0.0f, 0.0f, 1.0f };

	// ヒットSEのボリューム
	float HitSEVolume = 0.5f;

	// 弾ビルボードエフェクト
	GameObject* billboardObject = nullptr;
	int bulletSheetColumns = 6;
	int bulletSheetRows = 6;
	int bulletSheetFrameCount = 31;
	float bulletAnimationInterval = 0.08f;

	void Start() override;
	void Update() override;

private:
	float m_LifeTimer = 0.0f;
	Vector3 m_StartPos{};
	int m_HitSE = -1;

	// 弾ビルボードアニメーション
	float m_BulletAnimationTimer = 0.0f;
	size_t m_BulletAnimationIndex = 0;
	void AdvanceBulletBillboardAnimation(float deltaTime);
	void ApplyBulletFrameUV(size_t frameIndex);
};

#endif
