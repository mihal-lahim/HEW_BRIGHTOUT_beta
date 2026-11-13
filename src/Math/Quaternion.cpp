#include "Quaternion.h"
#include "Vector3.h"
#include <cmath>

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

Vector3 Quaternion::ToEulerAngles() const
{
	// クオータニオン成分取得 (DirectX の XMFLOAT4 は x,y,z,w の順)
	DirectX::XMFLOAT4 q = Quat.ToXMFLOAT4();
	float qx = q.x;
	float qy = q.y;
	float qz = q.z;
	float qw = q.w;

	// 以下は標準的な Tait-Bryan (X = pitch, Y = yaw, Z = roll) への変換式
	// pitch (X)
	float t0 = +2.0f * (qw * qx + qy * qz);
	float t1 = +1.0f - 2.0f * (qx * qx + qy * qy);
	float pitch = std::atan2(t0, t1);

	// yaw (Y)
	float t2 = +2.0f * (qw * qy - qz * qx);
	// 数値誤差で範囲を超えないよう clamping
	if (t2 > 1.0f) t2 = 1.0f;
	if (t2 < -1.0f) t2 = -1.0f;
	float yaw = std::asin(t2);

	// roll (Z)
	float t3 = +2.0f * (qw * qz + qx * qy);
	float t4 = +1.0f - 2.0f * (qy * qy + qz * qz);
	float roll = std::atan2(t3, t4);

	// ラジアン→度に変換し、Vector3(x=Pitch, y=Yaw, z=Roll) で返す
	return Vector3(
		XMConvertToDegrees(pitch),
		XMConvertToDegrees(yaw),
		XMConvertToDegrees(roll)
	);
}

Quaternion Quaternion::FromEulerAngles(const Vector3& eulerAngles)
{
	// 引数は (x=Pitch, y=Yaw, z=Roll) [度]
	// DirectX の XMQuaternionRotationRollPitchYaw は引数順が (pitch, yaw, roll)（ラジアン）
	float pitchRad = XMConvertToRadians(eulerAngles.x);
	float yawRad   = XMConvertToRadians(eulerAngles.y);
	float rollRad  = XMConvertToRadians(eulerAngles.z);

	Quaternion result;
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(pitchRad, yawRad, rollRad);
	result.Quat.FromXMVECTOR(quatVec);

	return result;
}
