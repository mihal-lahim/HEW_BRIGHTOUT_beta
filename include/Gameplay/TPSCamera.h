#ifndef TPS_CAMERA_H
#define TPS_CAMERA_H


#include <DirectXMath.h>
#include "ScriptComponent.h"
#include "GameObject.h"

// TPSカメラ設定構造体
struct TPSCameraCtx
{
    float Height = 1.0f;

    float SensitivityX = 1.5f; // 水平回転感度
	float SensitivityY = 1.5f; // 垂直（高さ）調整感度
	float FollowSpeed = 3.0f; // 自動追従の速度
	float Distance = 0.2f;
};


class TPSCamera : public ScriptComponent
{
public:
	TPSCamera(GameObject* target, TPSCameraCtx ctx = {})
		: m_target(target), m_ctx(ctx)
	{
	}

	// カメラ回転処理
	void Rotate(float inputX, float inputY);

	// TPSカメラ更新処理
	void Update() override;

private:
	GameObject* m_target = nullptr; // 注視対象オブジェクト

	TPSCameraCtx m_ctx; // カメラ設定値

	float m_angleX = 0.0f; // 水平回転角度
	float m_angleY = 0.0f; // 垂直（高さ）調整角度

	Vector3 m_Dest = {}; // 目標位置
};

#endif