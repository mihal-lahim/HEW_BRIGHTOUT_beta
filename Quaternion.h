
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
	Quaternion& Identity();

	// XYZ軸回転を設定する
	Quaternion& SetEulerX(float angle);
	Quaternion& SetEulerY(float angle);
	Quaternion& SetEulerZ(float angle);

	// 任意軸回転を設定する
	Quaternion& SetAngleAxis(float angle, const DirectX::XMVECTOR& axis);

	// Roll-Pitch-Yawの順で回転を設定する
	Quaternion& SetRollPitchYaw(float roll, float pitch, float yaw);

	// クオータニオンの乗算
	const Quaternion& operator*(const Quaternion& other) const;
};





#endif
