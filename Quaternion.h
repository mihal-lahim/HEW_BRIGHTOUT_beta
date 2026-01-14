#ifndef QUATERNION_H
#define QUATERNION_H

#include <DirectXMath.h>

class Quaternion
{
public:
	// クオータニオンの内部表現
	DirectX::XMFLOAT4 Quat;

	Quaternion();
	Quaternion(const DirectX::XMFLOAT4& quat) : Quat(quat) {}
	~Quaternion() = default;

	// 単位クオータニオンを取得する
	static Quaternion Identity();

	// XYZ軸回転を設定する
	static Quaternion SetEulerX(float angle);
	static Quaternion SetEulerY(float angle);
	static Quaternion SetEulerZ(float angle);

	// 任意軸回転を設定する
	static Quaternion SetAngleAxis(float angle, const DirectX::XMVECTOR& axis);

	// Roll-Pitch-Yawの順で回転を設定する
	static Quaternion SetRollPitchYaw(float roll, float pitch, float yaw);

	// クオータニオンの乗算
	Quaternion operator*(const Quaternion& other) const;
	Quaternion& operator*=(const Quaternion& other);
};





#endif
