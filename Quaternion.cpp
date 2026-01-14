#include "Quaternion.h"

using namespace DirectX;

Quaternion::Quaternion()
{
	// 単位クオータニオンで初期化
	XMVECTOR quatVec = XMQuaternionIdentity();
	XMStoreFloat4(&Quat, quatVec);

}

Quaternion Quaternion::Identity()
{
	// 単位クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionIdentity();
	XMStoreFloat4(&result.Quat, quatVec);

	return result;
}

Quaternion Quaternion::SetEulerX(float angle)
{
	// X軸回転用クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(angle, 0.0f, 0.0f);
	XMStoreFloat4(&result.Quat, quatVec);

	return result;
}

Quaternion Quaternion::SetEulerY(float angle)
{
	// Y軸回転用クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(0.0f, angle, 0.0f);
	XMStoreFloat4(&result.Quat, quatVec);

	return result;
}

Quaternion Quaternion::SetEulerZ(float angle)
{
	// Z軸回転用クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, angle);
	XMStoreFloat4(&result.Quat, quatVec);

	return result;
}

Quaternion Quaternion::SetAngleAxis(float angle, const DirectX::XMVECTOR& axis)
{
	// 任意軸回転用クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionRotationAxis(axis, angle);
	XMStoreFloat4(&result.Quat, quatVec);

	return result;
}

Quaternion Quaternion::SetRollPitchYaw(float roll, float pitch, float yaw)
{
	// Roll-Pitch-Yaw回転用クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);
	XMStoreFloat4(&result.Quat, quatVec);

	return result;
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
	// クオータニオンの乗算
	Quaternion result;

	XMVECTOR quatA = XMLoadFloat4(&Quat);
	XMVECTOR quatB = XMLoadFloat4(&other.Quat);

	XMVECTOR quatResult = XMQuaternionMultiply(quatA, quatB);
	XMStoreFloat4(&result.Quat, quatResult);

	return result;
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
	// クオータニオンの乗算代入
	XMVECTOR quatA = XMLoadFloat4(&Quat);
	XMVECTOR quatB = XMLoadFloat4(&other.Quat);
	XMVECTOR quatResult = XMQuaternionMultiply(quatA, quatB);
	XMStoreFloat4(&Quat, quatResult);

	return *this;
}
