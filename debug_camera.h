//////////////////////////////
//debugCamera.h
//Author: Hiroshi Kasiwagi
//Date: 2025/10/28
////////////////////////////////
#ifndef DEBUG_CAMERA_H
#define DEBUG_CAMERA_H
#include "camera.h"
#include <DirectXMath.h>

class DebugCamera : public Camera
{
public:

	DebugCamera() = default;

	//キー入力による更新
	void Update(double elapsed_time)override;
};

#endif // DEBUG_CAMERA_H