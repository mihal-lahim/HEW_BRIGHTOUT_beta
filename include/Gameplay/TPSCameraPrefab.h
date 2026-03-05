#ifndef TPS_CAMERA_PREFAB_H
#define TPS_CAMERA_PREFAB_H

#include "Prefab.h"
#include "TPSCamera.h"
#include "GameObject.h"
#include "Camera.h"

class TPSCameraPrefab : public Prefab
{
public:
	TPSCameraPrefab() = default;
	virtual ~TPSCameraPrefab() = default;

	GameObject* Target = nullptr;

	// インスタンス化メソッド
	virtual void Instantiate(GameObject& gameObject) override
	{
		gameObject.AddComponent<TPSCamera>(Target);
		gameObject.AddComponent<Camera>();
	}
};


#endif
