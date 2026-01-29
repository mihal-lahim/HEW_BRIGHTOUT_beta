//////////////////////////////
//debugCamera.h
//Author: Hiroshi Kasiwagi
//Date: 2025/10/28
////////////////////////////////
#ifndef DEBUG_CAMERA_H
#define DEBUG_CAMERA_H
#include "Camera.h"
#include "Keyboard.h"

class DebugCamera : public Camera
{
private:
	Keyboard* m_Keyboard = nullptr; //キーボード入力コンポーネント

public:
	DebugCamera();

	//キー入力による更新
	void Update() override;
};

#endif // DEBUG_CAMERA_H