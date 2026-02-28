

#include "Vector2.h"
#include "Quaternion.h"

using namespace DirectX;


Vector2 Vector2::operator+(const Vector2& other) const
{
	XMVECTOR result = XMVectorAdd(ToXMVECTOR(), other.ToXMVECTOR());
	Vector2 vec;
	vec.FromXMVECTOR(result);
	return vec;
}


Vector2 Vector2::operator-(const Vector2& other) const
{
	XMVECTOR result = XMVectorSubtract(ToXMVECTOR(), other.ToXMVECTOR());
	Vector2 vec;
	vec.FromXMVECTOR(result);
	return vec;
}


Vector2 Vector2::operator*(float scalar) const
{
	XMVECTOR result = XMVectorScale(ToXMVECTOR(), scalar);
	Vector2 vec;
	vec.FromXMVECTOR(result);
	return vec;
}


bool Vector2::IsZero() const
{
	return XMVector2Equal(ToXMVECTOR(), XMVectorZero());
}


float Vector2::Length() const
{
	return XMVectorGetX(XMVector2Length(ToXMVECTOR()));
}


Vector2 Vector2::Normalize() const
{
	if (IsZero())
		return *this;

	Vector2 result;
	result.FromXMVECTOR(XMVector2Normalize(ToXMVECTOR()));
	return result;
}


float Vector2::Dot(const Vector2& other) const
{
	return XMVectorGetX(XMVector2Dot(ToXMVECTOR(), other.ToXMVECTOR()));
}


DirectX::XMFLOAT2 Vector2::ToXMFLOAT2() const
{
	return DirectX::XMFLOAT2{ x, y };
}


DirectX::XMVECTOR Vector2::ToXMVECTOR() const
{
	DirectX::XMFLOAT2 temp = ToXMFLOAT2();
	return XMLoadFloat2(&temp);
}


void Vector2::FromXMFLOAT2(const DirectX::XMFLOAT2& vec)
{
	x = vec.x;
	y = vec.y;
}


void Vector2::FromXMVECTOR(const DirectX::XMVECTOR& vec)
{
	DirectX::XMFLOAT2 temp;
	XMStoreFloat2(&temp, vec);
	x = temp.x;
	y = temp.y;
}


Vector2& Vector2::operator+=(const Vector2& other)
{
	*this = *this + other;
	return *this;
}


Vector2& Vector2::operator-=(const Vector2& other)
{
	*this = *this - other;
	return *this;
}


Vector2& Vector2::operator*=(float scalar)
{
	*this = *this * scalar;
	return *this;
}
