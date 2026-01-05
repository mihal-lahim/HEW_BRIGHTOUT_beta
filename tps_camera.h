#ifndef TPS_CAMERA_H
#define TPS_CAMERA_H
#include "camera.h"
#include <DirectXMath.h>

// TPSカメラ設定構造体
struct TPSCameraCtx
{
    float Height = 10.0f;

    float SensitivityX = 4.0f; // 水平回転感度
	float SensitivityY = 2.0f; // 垂直（高さ）調整感度
	float FollowSpeed = 3.0f; // 自動追従の速度
	float MinDistance = 2.0f; // プレイヤーとの最小距離
	float MaxDistance = 15.0f; // プレイヤーとの最大距離
};

class TPSCamera : public Camera
{
private:
	GameObject* m_Target = nullptr; // 注視対象オブジェクト
	TPSCameraCtx m_Ctx; // カメラ設定値

	float Distance = 8.0f;

	float m_AngleX = 0.0f; // 水平回転角度
	float m_AngleY = 0.0f; // 垂直（高さ）調整角度

	DirectX::XMVECTOR m_Dest{}; // 目標位置

public:
    TPSCamera(GameObject* target, TPSCameraCtx ctx = {})
		: m_Target(target), m_Ctx(ctx)
	{}

	// カメラ回転処理
	void Rotate(float inputX, float inputY);

	// TPSカメラ更新処理
    void Update(double elapsed_time) override;
};

#endif