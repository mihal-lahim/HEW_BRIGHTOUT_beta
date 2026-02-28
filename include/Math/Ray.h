

#ifndef RAY_H
#define RAY_H

#include "Vector3.h"
#include "PhysicsSystem.h"

class PhysicsSystem;
class PhysicsBody;

struct Ray
{
public:
	// ヒットしたかどうか
	bool IsHit = false;

	// ヒットした位置
	Vector3 HitPosition{};

	// ヒット位置までの距離
	float HitDistance = -1.0f;

	// ヒットした法線
	Vector3 HitNormal{};

	// ヒットしたオブジェクト
	PhysicsBody* HitObject = nullptr;


	// コンストラクタ
	// 引数：始点、終点
	Ray(Vector3 from, Vector3 direction)
		: m_From(from), m_Direction(direction)
	{}

	virtual ~Ray() = default;

private:
	// レイの始点と終点
	Vector3 m_From{};
	Vector3 m_Direction{};

	// ヒット情報リセットメソッド
	void Reset()
	{
		IsHit = false;
		HitPosition = Vector3{};
		HitDistance = -1.0f;
		HitNormal = Vector3{};
		HitObject = nullptr;
	}

	friend class PhysicsSystem;
};



#endif

