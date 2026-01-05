/*==============================================================================

   カメラ [camera.h]
														 Author : sumi rintarou
														 Date   : 2025/10/28
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject.h"

// カメラ設定値構造体
struct CameraCtx
{
	float Fov  = 60.0f;          // 視野角
	float Near = 0.1f;         // ニアクリップ距離
	float Far = 1000.0f;          // ファークリップ距離

	// ビューポートサイズ（どちらかが-1.0fの場合はバックバッファサイズを使用）
	float ViewportWidth = -1.0f;  // ビューポート幅
	float ViewportHeight = -1.0f; // ビューポート高さ

	int Priority = 0;       // カメラの優先度（数値が小さいほど優先される）
};

class Camera : public GameObject
{
protected:
	// カメラ設定値
	CameraCtx m_Ctx{};
public:
	Camera() = default;
	virtual ~Camera() = default;

	// 行列を定数バッファに設定するメソッド
	void SetMatrix() const;

	// カメラ設定値の取得・設定メソッド
	const CameraCtx& GetCameraCtx() const { return m_Ctx; }
	void SetCameraCtx(const CameraCtx& ctx) { m_Ctx = ctx; }
};

#endif