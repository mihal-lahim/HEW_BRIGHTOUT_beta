#ifndef QUATERNION_H
#define QUATERNION_H

#include <DirectXMath.h>
#include "Vector4.h"

class Vector3;

class Quaternion
{
public:
	// クオータニオンの内部表現
	Vector4 Quat;

	Quaternion();
	Quaternion(const Vector4& quat) : Quat(quat) {}
	~Quaternion() = default;

	// DirectX::XMFLOAT4への変換
	DirectX::XMFLOAT4 ToXMFLOAT4() const;

	// XMVECTORへの変換
	DirectX::XMVECTOR ToXMVECTOR() const;

	// XMFLOAT4からの変換
	void FromXMFLOAT4(const DirectX::XMFLOAT4& vec);

	// XMVECTORからの変換
	void FromXMVECTOR(const DirectX::XMVECTOR& vec);

	// DirectX::XMMATRIXへの変換
	DirectX::XMMATRIX ToXMMATRIX() const;

	// XMMATRIXからの変換
	void FromXMMATRIX(const DirectX::XMMATRIX& mat);


	// 単位クオータニオンを取得する
	static Quaternion Identity();

	// XYZ軸回転を設定する
	static Quaternion SetEulerX(float angle);
	static Quaternion SetEulerY(float angle);
	static Quaternion SetEulerZ(float angle);

	// 任意軸回転を設定する
	static Quaternion SetAngleAxis(float angle, const Vector3& axis);

	// Roll-Pitch-Yawの順で回転を設定する
	static Quaternion SetRollPitchYaw(float roll, float pitch, float yaw);

	// クオータニオンの乗算
	Quaternion operator*(const Quaternion& other) const;
	Quaternion& operator*=(const Quaternion& other);
};





#endif
