
#ifndef RAY_H

#include <DirectXMath.h>
#include "GameObject.h"
#include "GameManager.h"
#include "PhysicsSystem.h"

class PhysicsSystem;

class Ray : public GameObject
{
private:
	// レイの始点と終点
	DirectX::XMFLOAT3 m_From{};
	DirectX::XMFLOAT3 m_Direction{};

public:
	// ヒットしたかどうか
	bool IsHit = false;

	// ヒットした位置
	DirectX::XMFLOAT3 HitPosition{};

	// ヒット位置までの距離
	float HitDistance = -1.0f;

	// ヒットした法線
	DirectX::XMFLOAT3 HitNormal{};

	// ヒットしたオブジェクト
	GameObject* HitObject = nullptr;


	// コンストラクタ
	// 引数：始点、終点
	Ray(DirectX::XMFLOAT3 from, DirectX::XMFLOAT3 direction)
		: m_From(from), m_Direction(direction)
	{}

	virtual ~Ray() = default;

	// レイキャスト実行メソッド
	void RayCast(float distance) { GetPhysicsSystem().RayCast(*this, distance); }

	friend class PhysicsSystem;
};



#endif

