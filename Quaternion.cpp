#include "Quaternion.h"
#include "Vector3.h"

using namespace DirectX;

Quaternion::Quaternion()
{
	// 単位クオータニオンで初期化
	XMVECTOR quatVec = XMQuaternionIdentity();
	Quat.FromXMVECTOR(quatVec);
}

DirectX::XMFLOAT4 Quaternion::ToXMFLOAT4() const
{
	return Quat.ToXMFLOAT4();
}

DirectX::XMVECTOR Quaternion::ToXMVECTOR() const
{
	return Quat.ToXMVECTOR();
}

void Quaternion::FromXMFLOAT4(const DirectX::XMFLOAT4& vec)
{
	return Quat.FromXMFLOAT4(vec);
}

void Quaternion::FromXMVECTOR(const DirectX::XMVECTOR& vec)
{
	return Quat.FromXMVECTOR(vec);
}

DirectX::XMMATRIX Quaternion::ToXMMATRIX() const
{
	// クォータニオンから回転行列を作成
	XMVECTOR quatVec = Quat.ToXMVECTOR();
	return XMMatrixRotationQuaternion(quatVec);
}

void Quaternion::FromXMMATRIX(const DirectX::XMMATRIX& mat)
{
	// 回転行列からクォータニオンを作成
	XMVECTOR quatVec = XMQuaternionRotationMatrix(mat);
	Quat.FromXMVECTOR(quatVec);
}

Quaternion Quaternion::Identity()
{
	// 単位クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionIdentity();
	result.Quat.FromXMVECTOR(quatVec);

	return result;
}

Quaternion Quaternion::SetEulerX(float angle)
{
	// 角度をラジアンに変換
	angle = XMConvertToRadians(angle);

	// X軸回転用クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(angle, 0.0f, 0.0f);
	result.Quat.FromXMVECTOR(quatVec);

	return result;
}

Quaternion Quaternion::SetEulerY(float angle)
{
	// 角度をラジアンに変換
	angle = XMConvertToRadians(angle);

	// Y軸回転用クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(0.0f, angle, 0.0f);
	result.Quat.FromXMVECTOR(quatVec);

	return result;
}

Quaternion Quaternion::SetEulerZ(float angle)
{
	// 角度をラジアンに変換
	angle = XMConvertToRadians(angle);

	// Z軸回転用クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, angle);
	result.Quat.FromXMVECTOR(quatVec);

	return result;
}

Quaternion Quaternion::SetAngleAxis(float angle, const Vector3& axis)
{
	// 角度をラジアンに変換
	angle = XMConvertToRadians(angle);

	// 任意軸回転用クオータニオンを作成
	Quaternion result;
	XMVECTOR axisVec = axis.ToXMVECTOR();
	XMVECTOR quatVec = XMQuaternionRotationAxis(axisVec, angle);
	result.Quat.FromXMVECTOR(quatVec);

	return result;
}

Quaternion Quaternion::SetRollPitchYaw(float roll, float pitch, float yaw)
{
	// 角度をラジアンに変換
	roll = XMConvertToRadians(roll);
	pitch = XMConvertToRadians(pitch);
	yaw = XMConvertToRadians(yaw);

	// Roll-Pitch-Yaw回転用クオータニオンを作成
	Quaternion result;
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);
	result.Quat.FromXMVECTOR(quatVec);

	return result;
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
	// クオータニオンの乗算
	Quaternion result;

	XMVECTOR quatA = Quat.ToXMVECTOR();
	XMVECTOR quatB = other.Quat.ToXMVECTOR();

	XMVECTOR quatResult = XMQuaternionMultiply(quatA, quatB);
	result.Quat.FromXMVECTOR(quatResult);

	return result;
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
	// クオータニオンの乗算代入
	XMVECTOR quatA = Quat.ToXMVECTOR();
	XMVECTOR quatB = other.Quat.ToXMVECTOR();
	XMVECTOR quatResult = XMQuaternionMultiply(quatA, quatB);
	Quat.FromXMVECTOR(quatResult);

	return *this;
}
