//////////////////////////////
//debugCamera.cpp
//Author: Hiroshi Kasiwagi
//Date: 2025/10/28
////////////////////////////////
#include "include/DebugTest/DebugCamera.h"
#include "GameTime.h"
#include "GameObject.h"
#include "InputSystem.h"
#include <DirectXMath.h>

using namespace DirectX;


void DebugCamera::Update()
{
	//速度設定
	const float MOVE_SPEED = static_cast<float>(2.0f * Time::DeltaTime()); //移動速度
	const float ROTATION_SPEED = static_cast<float>(60.0f * Time::DeltaTime()); //回転速度

	Keyboard& keyboard = input().keyboard();

	//オーナーのTransform取得
	Transform& camtf = gameObject().transform();

	//リセット
	if (keyboard.IsDown(KK_TAB))
	{
		camtf.position() = {0.0f, 0.0f, 0.0f};
		camtf.rotation() = Quaternion::Identity();
	}


	//右回転
	if (keyboard.IsPressed(KK_RIGHT))
		camtf.rotation() *= Quaternion::SetEulerY(ROTATION_SPEED);

	//左回転
	if (keyboard.IsPressed(KK_LEFT))
		camtf.rotation() *= Quaternion::SetEulerY(-ROTATION_SPEED);

	//上回転
	if (keyboard.IsPressed(KK_UP))
		camtf.rotation() *= Quaternion::SetEulerX(ROTATION_SPEED);

	//下回転
	if (keyboard.IsPressed(KK_DOWN))
		camtf.rotation() *= Quaternion::SetEulerX(-ROTATION_SPEED);

	//きりもみ右回転
	if (keyboard.IsPressed(KK_P))
		camtf.rotation() *= Quaternion::SetEulerZ(ROTATION_SPEED);

	//きりもみ左回転
	if (keyboard.IsPressed(KK_O))
		camtf.rotation() *= Quaternion::SetEulerZ(ROTATION_SPEED);

	//前進
	if (keyboard.IsPressed(KK_W))
		camtf.position().z += MOVE_SPEED;

	//右移動
	if (keyboard.IsPressed(KK_D))
		camtf.position().x += MOVE_SPEED;

	//左移動
	if (keyboard.IsPressed(KK_A))
		camtf.position().x -= MOVE_SPEED;

	//後退
	if (keyboard.IsPressed(KK_S))
		camtf.position().z -= MOVE_SPEED;

	//上移動
	if (keyboard.IsPressed(KK_SPACE))
		camtf.position().y += MOVE_SPEED;

	//下移動
	if (keyboard.IsPressed(KK_LEFTSHIFT))
		camtf.position().y -= MOVE_SPEED;
}
