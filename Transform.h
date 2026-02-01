

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vector3.h"
#include "Quaternion.h"
#include "Component.h"


class Transform : public Component
{
private:
	// 位置
	Vector3 m_position{};
	// 回転（クオータニオン）
	Quaternion m_rotation{};
	// スケール
	Vector3 m_scale{ 1.0f, 1.0f, 1.0f };
public:

	Transform() = default;
	~Transform() = default;

	// 位置の取得・設定メソッド
	Vector3& position() { return m_position; }

	// 回転の取得・設定メソッド
	Quaternion& rotation() { return m_rotation; }

	// スケールの取得・設定メソッド
	Vector3& scale() { return m_scale; }
};


#endif
