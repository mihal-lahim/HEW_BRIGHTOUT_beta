

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vector3.h"
#include "Quaternion.h"


class Transform
{
public:
	// 位置
	Vector3 Position{};
	// 回転（クオータニオン）
	Quaternion Rotation{};
	// スケール
	Vector3 Scale{ 1.0f, 1.0f, 1.0f };

	Transform() = default;
	~Transform() = default;
};


#endif
