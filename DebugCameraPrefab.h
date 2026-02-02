#ifndef DEBUG_CAMERA_PREFAB_H
#define DEBUG_CAMERA_PREFAB_H

#include "Prefab.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "GameObject.h"

class DebugCameraPrefab : public Prefab
{
public:
	virtual ~DebugCameraPrefab() = default;

	// インスタンス化メソッド
	virtual void Instantiate(GameObject& gameObject) const override
	{
		// DebugCameraコンポーネントを追加
		gameObject.AddComponent<DebugCamera>();
		// Cameraコンポーネントを追加
		gameObject.AddComponent<Camera>();
	}
};


#endif
