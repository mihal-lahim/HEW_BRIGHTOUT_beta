/*==============================================================================

   カメラ [camera.h]
														 Author : sumi rintarou
														 Date   : 2025/10/28
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef CAMERA_H
#define CAMERA_H


#include "Component.h"
#include "GameManager.h"
#include "GameObject.h"


class Camera : public GameObject
{
public:

	float Fov = 60.0f;          // 視野角
	float Near = 0.1f;         // ニアクリップ距離
	float Far = 1000.0f;          // ファークリップ距離

	// ビューポートサイズ（どちらかが-1.0fの場合はバックバッファサイズを使用）
	float ViewportWidth = -1.0f;  // ビューポート幅
	float ViewportHeight = -1.0f; // ビューポート高さ

	int Priority = 0;       // カメラの優先度（数値が小さいほど優先される）



	Camera() { GameManager::GetRenderSystem().RegisterCamera(this); }
	virtual ~Camera() { GameManager::GetRenderSystem().UnregisterCamera(this); }

	// 行列を定数バッファに設定するメソッド
	void SetMatrix() const;
};

#endif