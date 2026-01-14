
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <DirectXMath.h>
#include "Quaternion.h"


class Transform
{
public:
	// 位置
	DirectX::XMFLOAT3 Position{};
	// 回転（クオータニオン）
	Quaternion Rotation{};
	// スケール
	DirectX::XMFLOAT3 Scale{ 1.0f,1.0f,1.0f };

	Transform() = default;
	~Transform() = default;
};


#endif
