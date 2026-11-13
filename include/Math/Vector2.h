#ifndef VECTOR2_H
#define VECTOR2_H


#include <DirectXMath.h>

class Vector2
{
public:
	float x;
	float y;

	// デフォルトコンストラクタ
	Vector2() : x(0.0f), y(0.0f) {}

	// 引数付きコンストラクタ
	Vector2(float x, float y)
		: x(x), y(y)
	{}

	// DirectX::XMFLOAT2への変換
	DirectX::XMFLOAT2 ToXMFLOAT2() const;

	// XMVECTORへの変換
	DirectX::XMVECTOR ToXMVECTOR() const;

	// XMFLOAT2からの変換
	void FromXMFLOAT2(const DirectX::XMFLOAT2& vec);

	// XMVECTORからの変換
	void FromXMVECTOR(const DirectX::XMVECTOR& vec);


	// ベクトルの加算
	Vector2 operator+(const Vector2& other) const;

	// ベクトルの減算
	Vector2 operator-(const Vector2& other) const;

	// スカラー倍
	Vector2 operator*(float scalar) const;

	// 複合代入演算子
	Vector2& operator+=(const Vector2& other);
	Vector2& operator-=(const Vector2& other);
	Vector2& operator*=(float scalar);


	// 0ベクトルかどうかの判定
	bool IsZero() const;

	// ベクトルの長さを計算
	float Length() const;

	// 正規化
	Vector2 Normalize() const;

	// 内積の計算
	float Dot(const Vector2& other) const;
};



#endif
