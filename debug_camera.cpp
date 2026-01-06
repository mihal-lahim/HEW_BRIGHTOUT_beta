//////////////////////////////
//debugCamera.cpp
//Author: Hiroshi Kasiwagi
//Date: 2025/10/28
////////////////////////////////
#include "debug_camera.h"
#include "key_logger.h"

using namespace DirectX;

void DebugCamera::Update(double elapsed_time)
{
	//速度設定
	const float MOVE_SPEED = static_cast<float>(2.0f * elapsed_time); //移動速度
	const float ROTATION_SPEED = static_cast<float>(60.0f * elapsed_time); //回転速度

	//リセット
	if (KeyLogger_IsTrigger(KK_TAB))
	{
		m_Position = { 0.0f, 0.0f, 0.0f };
		m_Rotation = { 0.0f, 0.0f, 0.0f };
	}

	//ズームアウト
	if (KeyLogger_IsPressed(KK_K))
		m_Ctx.Fov += 0.1f;

	//ズームイン
	if (KeyLogger_IsPressed(KK_L))
	{
		m_Ctx.Fov -= 0.1f;

		if (m_Ctx.Fov <= 0)
			m_Ctx.Fov = 0.1f;
	}

	//右回転
	if (KeyLogger_IsPressed(KK_RIGHT))
		m_Rotation.y += ROTATION_SPEED;

	//左回転
	if (KeyLogger_IsPressed(KK_LEFT))
		m_Rotation.y -= ROTATION_SPEED;

	//上回転
	if (KeyLogger_IsPressed(KK_UP))
		m_Rotation.x += ROTATION_SPEED;

	//下回転
	if (KeyLogger_IsPressed(KK_DOWN))
		m_Rotation.x -= ROTATION_SPEED;

	//きりもみ右回転
	if (KeyLogger_IsPressed(KK_P))
		m_Rotation.z += ROTATION_SPEED;

	//きりもみ左回転
	if (KeyLogger_IsPressed(KK_O))
		m_Rotation.z -= ROTATION_SPEED;

	//前進
	if (KeyLogger_IsPressed(KK_W))
		m_Position.z += MOVE_SPEED;

	//右移動
	if (KeyLogger_IsPressed(KK_D))
		m_Position.x += MOVE_SPEED;

	//左移動
	if (KeyLogger_IsPressed(KK_A))
		m_Position.x -= MOVE_SPEED;

	//後退
	if (KeyLogger_IsPressed(KK_S))
		m_Position.z -= MOVE_SPEED;

	//上移動
	if (KeyLogger_IsPressed(KK_SPACE))
		m_Position.y += MOVE_SPEED;

	//下移動
	if (KeyLogger_IsPressed(KK_LEFTSHIFT))
		m_Position.y -= MOVE_SPEED;
}
