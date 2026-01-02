/*==============================================================================

   カメラ [camera.h]
														 Author : sumi rintarou
														 Date   : 2025/10/28
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject.h"

class Camera : public GameObject
{
protected:
	float m_Fov = 60.0f; // 視野角
	float m_Near = 0.1f; // ニアクリップ距離
	float m_Far = 1000.0f; // ファークリップ距離

	float m_AspectRatio = -1.0f; // アスペクト比（-1.0f の場合は自動計算）
public:
	Camera() = default;
	virtual ~Camera() = default;

	// 更新メソッド
	virtual void Update(double elapsedTime);

	// 行列を定数バッファに設定するメソッド
	virtual void SetMatrix() const;
};

#endif