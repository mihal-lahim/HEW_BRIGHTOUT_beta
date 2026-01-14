#include "Quaternion.h"

using namespace DirectX;

Quaternion::Quaternion()
{
	// 単位クオータニオンで初期化
	XMVECTOR quatVec = XMQuaternionIdentity();
	XMStoreFloat4(&Quat, quatVec);

}

Quaternion& Quaternion::Identity()
{
	// 単位クオータニオンを設定
	XMVECTOR quatVec = XMQuaternionIdentity();
	XMStoreFloat4(&Quat, quatVec);

	return *this;
}

Quaternion& Quaternion::SetEulerX(float angle)
{
	// X軸回転用クオータニオンを作成
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(angle, 0.0f, 0.0f);
	XMStoreFloat4(&Quat, quatVec);

	return *this;
}

Quaternion& Quaternion::SetEulerY(float angle)
{
	// Y軸回転用クオータニオンを作成
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(0.0f, angle, 0.0f);
	XMStoreFloat4(&Quat, quatVec);

	return *this;
}

Quaternion& Quaternion::SetEulerZ(float angle)
{
	// Z軸回転用クオータニオンを作成
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, angle);
	XMStoreFloat4(&Quat, quatVec);

	return *this;
}

Quaternion& Quaternion::SetAngleAxis(float angle, const DirectX::XMVECTOR& axis)
{
	// 任意軸回転用クオータニオンを作成
	XMVECTOR quatVec = XMQuaternionRotationAxis(axis, angle);
	XMStoreFloat4(&Quat, quatVec);

	return *this;
}

Quaternion& Quaternion::SetRollPitchYaw(float roll, float pitch, float yaw)
{
	// Roll-Pitch-Yaw回転用クオータニオンを作成
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	XMStoreFloat4(&Quat, quatVec);

	return *this;
}

const Quaternion& Quaternion::operator*(const Quaternion& other) const
{
	// クオータニオンの乗算
	Quaternion result;

	XMVECTOR quatA = XMLoadFloat4(&Quat);
	XMVECTOR quatB = XMLoadFloat4(&other.Quat);

	XMVECTOR quatResult = XMQuaternionMultiply(quatA, quatB);
	XMStoreFloat4(&result.Quat, quatResult);

	return result;
}
