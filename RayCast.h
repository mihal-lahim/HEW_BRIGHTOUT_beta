
#ifndef RAY_CAST_H

#include <DirectXMath.h>
#include "GameObject.h"

class PhysicsSystem;

class RayCast : public GameObject
{

private:
	// ヒットしたかどうか
	bool m_IsHit = false;

	// レイの始点と終点
	DirectX::XMFLOAT3 m_From{};
	DirectX::XMFLOAT3 m_To{};
	
	// ヒットした位置
	DirectX::XMFLOAT3 m_HitPosition{};

	// ヒット位置までの距離
	float m_HitDistance = -1.0f;

	// ヒットした法線
	DirectX::XMFLOAT3 m_HitNormal{};

	// ヒットしたオブジェクト
	GameObject* m_HitObject = nullptr;

public:

	// デフォルトコンストラクタ
	RayCast() = default;

	// コンストラクタ
	// 引数：始点、終点
	RayCast(DirectX::XMFLOAT3 from, DirectX::XMFLOAT3 to)
		: m_From(from), m_To(to)
	{}

	virtual ~RayCast() = default;

	// 始点・終点設定メソッド
	void SetFrom(DirectX::XMFLOAT3 from) { m_From = from; }
	void SetTo(DirectX::XMFLOAT3 to) { m_To = to; }

	// ヒットしたかどうかを取得
	bool IsHit() const { return m_IsHit; }

	// ヒット位置を取得
	DirectX::XMFLOAT3 GetHitPosition() const { return m_HitPosition; }

	// ヒット距離を取得
	float GetHitDistance() const { return m_HitDistance; }

	// ヒット法線を取得
	DirectX::XMFLOAT3 GetHitNormal() const { return m_HitNormal; }

	// ヒットオブジェクトを取得
	GameObject* GetHitObject() const { return m_HitObject; }

	friend class PhysicsSystem;
};



#endif

