


#include "Vector3.h"
#include "Quaternion.h"

using namespace DirectX;


Vector3 Vector3::operator+(const Vector3& other) const
{
	XMVECTOR result = XMVectorAdd(ToXMVECTOR(), other.ToXMVECTOR());
	Vector3 vec;
	vec.FromXMVECTOR(result);
	return vec;
}


Vector3 Vector3::operator-(const Vector3& other) const
{
	XMVECTOR result = XMVectorSubtract(ToXMVECTOR(), other.ToXMVECTOR());
	Vector3 vec;
	vec.FromXMVECTOR(result);
	return vec;
}


Vector3 Vector3::operator*(float scalar) const
{
	XMVECTOR result = XMVectorScale(ToXMVECTOR(), scalar);
	Vector3 vec;
	vec.FromXMVECTOR(result);
	return vec;
}


bool Vector3::IsZero() const
{
	return XMVector3Equal(ToXMVECTOR(), XMVectorZero());
}


float Vector3::Length() const
{
	return XMVectorGetX(XMVector3Length(ToXMVECTOR()));
}


Vector3 Vector3::Normalize() const
{
	if (IsZero())
		return *this;

	Vector3 result;
	result.FromXMVECTOR(XMVector3Normalize(ToXMVECTOR()));
	return result;
}


float Vector3::Dot(const Vector3& other) const
{
	return XMVectorGetX(XMVector3Dot(ToXMVECTOR(), other.ToXMVECTOR()));
}


float Vector3::Cross(const Vector3& other) const
{
	XMVECTOR cross = XMVector3Cross(ToXMVECTOR(), other.ToXMVECTOR());
	return XMVectorGetX(cross);
}


Vector3 Vector3::CrossVector(const Vector3& other) const
{
	XMVECTOR cross = XMVector3Cross(ToXMVECTOR(), other.ToXMVECTOR());
	Vector3 result;
	result.FromXMVECTOR(cross);
	return result;
}



DirectX::XMFLOAT3 Vector3::ToXMFLOAT3() const
{
	return DirectX::XMFLOAT3{ x, y, z };
}


DirectX::XMVECTOR Vector3::ToXMVECTOR() const
{
	DirectX::XMFLOAT3 temp = ToXMFLOAT3();
	return XMLoadFloat3(&temp);
}


void Vector3::FromXMFLOAT3(const DirectX::XMFLOAT3& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}


void Vector3::FromXMVECTOR(const DirectX::XMVECTOR& vec)
{
	DirectX::XMFLOAT3 temp;
	XMStoreFloat3(&temp, vec);
	x = temp.x;
	y = temp.y;
	z = temp.z;
}


Vector3& Vector3::operator+=(const Vector3& other)
{
	*this = *this + other;
	return *this;
}


Vector3& Vector3::operator-=(const Vector3& other)
{
	*this = *this - other;
	return *this;
}


Vector3& Vector3::operator*=(float scalar)
{
	*this = *this * scalar;
	return *this;
}


Vector3 Vector3::Rotate(const Quaternion& quat) const
{
	XMVECTOR rotated = XMVector3Rotate(ToXMVECTOR(), quat.ToXMVECTOR());
	Vector3 result;
	result.FromXMVECTOR(rotated);
	return result;
}

Vector3 Vector3::RotateAxis(const Vector3& axis, float angle) const
{
	Quaternion quat = Quaternion::SetAngleAxis(angle, axis);
	return Rotate(quat);
}
