

#include "Vector4.h"
#include "Quaternion.h"

using namespace DirectX;


Vector4 Vector4::operator+(const Vector4& other) const
{
	XMVECTOR result = XMVectorAdd(ToXMVECTOR(), other.ToXMVECTOR());
	Vector4 vec;
	vec.FromXMVECTOR(result);
	return vec;
}


Vector4 Vector4::operator-(const Vector4& other) const
{
	XMVECTOR result = XMVectorSubtract(ToXMVECTOR(), other.ToXMVECTOR());
	Vector4 vec;
	vec.FromXMVECTOR(result);
	return vec;
}


Vector4 Vector4::operator*(float scalar) const
{
	XMVECTOR result = XMVectorScale(ToXMVECTOR(), scalar);
	Vector4 vec;
	vec.FromXMVECTOR(result);
	return vec;
}


bool Vector4::IsZero() const
{
	return XMVector4Equal(ToXMVECTOR(), XMVectorZero());
}


float Vector4::Length() const
{
	return XMVectorGetX(XMVector4Length(ToXMVECTOR()));
}


Vector4 Vector4::Normalize() const
{
	if (IsZero())
		return *this;

	Vector4 result;
	result.FromXMVECTOR(XMVector4Normalize(ToXMVECTOR()));
	return result;
}


float Vector4::Dot(const Vector4& other) const
{
	return XMVectorGetX(XMVector4Dot(ToXMVECTOR(), other.ToXMVECTOR()));
}


DirectX::XMFLOAT4 Vector4::ToXMFLOAT4() const
{
	return DirectX::XMFLOAT4{ x, y, z, w };
}


DirectX::XMVECTOR Vector4::ToXMVECTOR() const
{
	DirectX::XMFLOAT4 temp = ToXMFLOAT4();
	return XMLoadFloat4(&temp);
}


void Vector4::FromXMFLOAT4(const DirectX::XMFLOAT4& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;
}


void Vector4::FromXMVECTOR(const DirectX::XMVECTOR& vec)
{
	DirectX::XMFLOAT4 temp;
	XMStoreFloat4(&temp, vec);
	x = temp.x;
	y = temp.y;
	z = temp.z;
	w = temp.w;
}


Vector4& Vector4::operator+=(const Vector4& other)
{
	*this = *this + other;
	return *this;
}


Vector4& Vector4::operator-=(const Vector4& other)
{
	*this = *this - other;
	return *this;
}


Vector4& Vector4::operator*=(float scalar)
{
	*this = *this * scalar;
	return *this;
}
