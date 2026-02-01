#ifndef CAMERA_PREFAB_H
#define CAMERA_PREFAB_H

#include "Prefab.h"
#include "Camera.h"
#include "GameObject.h"

class CameraPrefab : public Prefab
{
	void Instantiate(GameObject& gameObject) const override
	{
		gameObject.AddComponent<Camera>();
	}
};


#endif
