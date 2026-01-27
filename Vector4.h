#ifndef VECTOR4_H
#define VECTOR4_H


#include <DirectXMath.h>

class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	// デフォルトコンストラクタ
	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

	// 引数付きコンストラクタ
	Vector4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{}

	// DirectX::XMFLOAT4への変換
	DirectX::XMFLOAT4 ToXMFLOAT4() const;

	// XMVECTORへの変換
	DirectX::XMVECTOR ToXMVECTOR() const;

	// XMFLOAT4からの変換
	void FromXMFLOAT4(const DirectX::XMFLOAT4& vec);

	// XMVECTORからの変換
	void FromXMVECTOR(const DirectX::XMVECTOR& vec);


	// ベクトルの加算
	Vector4 operator+(const Vector4& other) const;

	// ベクトルの減算
	Vector4 operator-(const Vector4& other) const;

	// スカラー倍
	Vector4 operator*(float scalar) const;

	// 複合代入演算子
	Vector4& operator+=(const Vector4& other);
	Vector4& operator-=(const Vector4& other);
	Vector4& operator*=(float scalar);


	// 0ベクトルかどうかの判定
	bool IsZero() const;

	// ベクトルの長さを計算
	float Length() const;

	// 正規化（自分自身を変更）
	Vector4& Normalize();

	// 内積の計算
	float Dot(const Vector4& other) const;
};



#endif
