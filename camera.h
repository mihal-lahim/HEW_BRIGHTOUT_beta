/*==============================================================================

   カメラ [camera.h]
														 Author : sumi rintarou
														 Date   : 2025/10/28
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef CAMERA_H
#define CAMERA_H


#include "Component.h"
#include <DirectXMath.h>


class Camera : public Component
{
public:

	float Fov = 60.0f;          // 視野角
	float Near = 0.1f;         // ニアクリップ距離
	float Far = 1000.0f;          // ファークリップ距離

	// ビューポートサイズ（どちらかが-1.0fの場合はバックバッファサイズを使用）
	float ViewportWidth = -1.0f;  // ビューポート幅
	float ViewportHeight = -1.0f; // ビューポート高さ

	int Priority = 0;       // カメラの優先度（数値が小さいほど優先される）

	Camera() = default;
	virtual ~Camera() = default;

	// ビュー行列取得メソッド
	DirectX::XMMATRIX GetViewMatrix() const;

	// プロジェクション行列取得メソッド
	DirectX::XMMATRIX GetProjectionMatrix(float defaultWidth, float defaultHeight) const;
};

#endif