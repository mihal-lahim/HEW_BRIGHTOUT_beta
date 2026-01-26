//////////////////////////////
//debugCamera.cpp
//Author: Hiroshi Kasiwagi
//Date: 2025/10/28
////////////////////////////////
#include "DebugCamera.h"
#include "key_logger.h"
#include "Time.h"

using namespace DirectX;

void DebugCamera::Update()
{
	//速度設定
	const float MOVE_SPEED = static_cast<float>(2.0f * Time::DeltaTime()); //移動速度
	const float ROTATION_SPEED = static_cast<float>(60.0f * Time::DeltaTime()); //回転速度

	//オーナーのTransform取得
	Transform& camtf = transform;

	//リセット
	if (KeyLogger_IsTrigger(KK_TAB))
	{
		camtf.Position = { 0.0f, 0.0f, 0.0f };
		camtf.Rotation = Quaternion::Identity();
	}

	//ズームアウト
	if (KeyLogger_IsPressed(KK_K))
		Fov += 0.1f;

	//ズームイン
	if (KeyLogger_IsPressed(KK_L))
	{
		Fov -= 0.1f;

		if (Fov <= 0)
			Fov = 0.1f;
	}

	//右回転
	if (KeyLogger_IsPressed(KK_RIGHT))
		camtf.Rotation *= Quaternion::SetEulerY(ROTATION_SPEED);

	//左回転
	if (KeyLogger_IsPressed(KK_LEFT))
		camtf.Rotation *= Quaternion::SetEulerY(-ROTATION_SPEED);

	//上回転
	if (KeyLogger_IsPressed(KK_UP))
		camtf.Rotation *= Quaternion::SetEulerX(ROTATION_SPEED);

	//下回転
	if (KeyLogger_IsPressed(KK_DOWN))
		camtf.Rotation *= Quaternion::SetEulerX(-ROTATION_SPEED);

	//きりもみ右回転
	if (KeyLogger_IsPressed(KK_P))
		camtf.Rotation *= Quaternion::SetEulerZ(ROTATION_SPEED);

	//きりもみ左回転
	if (KeyLogger_IsPressed(KK_O))
		camtf.Rotation *= Quaternion::SetEulerZ(ROTATION_SPEED);

	//前進
	if (KeyLogger_IsPressed(KK_W))
		camtf.Position.z += MOVE_SPEED;

	//右移動
	if (KeyLogger_IsPressed(KK_D))
		camtf.Position.x += MOVE_SPEED;

	//左移動
	if (KeyLogger_IsPressed(KK_A))
		camtf.Position.x -= MOVE_SPEED;

	//後退
	if (KeyLogger_IsPressed(KK_S))
		camtf.Position.z -= MOVE_SPEED;

	//上移動
	if (KeyLogger_IsPressed(KK_SPACE))
		camtf.Position.y += MOVE_SPEED;

	//下移動
	if (KeyLogger_IsPressed(KK_LEFTSHIFT))
		camtf.Position.y -= MOVE_SPEED;
}
