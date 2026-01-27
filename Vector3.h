


#ifndef VECTOR3_H
#define VECTOR3_H


#include <DirectXMath.h>
#include "Quaternion.h"

class Vector3
{
public:
	float x;
	float y;
	float z;

	// デフォルトコンストラクタ
	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

	// 引数付きコンストラクタ
	Vector3(float x, float y, float z)
		: x(x), y(y), z(z)
	{}

	// DirectX::XMFLOAT3への変換
	DirectX::XMFLOAT3 ToXMFLOAT3() const;

	// XMVECTORへの変換
	DirectX::XMVECTOR ToXMVECTOR() const;

	// XMFLOAT3からの変換
	void FromXMFLOAT3(const DirectX::XMFLOAT3& vec);

	// XMVECTORからの変換
	void FromXMVECTOR(const DirectX::XMVECTOR& vec);


	// ベクトルの加算
	Vector3 operator+(const Vector3& other) const;

	// ベクトルの減算
	Vector3 operator-(const Vector3& other) const;

	// スカラー倍
	Vector3 operator*(float scalar) const;

	// 複合代入演算子
	Vector3& operator+=(const Vector3& other);
	Vector3& operator-=(const Vector3& other);
	Vector3& operator*=(float scalar);


	// 0ベクトルかどうかの判定
	bool IsZero() const;

	// ベクトルの長さを計算
	float Length() const;

	// 正規化（自分自身を変更）
	Vector3& Normalize();

	// 内積の計算
	float Dot(const Vector3& other) const;

	// 外積の計算
	float Cross(const Vector3& other) const;

	// Quaternionによる回転（自分自身を変更）
	Vector3& Rotate(const Quaternion& quat);

	// 任意軸周りの回転（自分自身を変更）
	Vector3& RotateAxis(const Vector3& axis, float angle);
};



#endif
