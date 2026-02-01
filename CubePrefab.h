#ifndef CUBE_PREFAB_H
#define CUBE_PREFAB_H

#include "Prefab.h"
#include "MeshRenderer.h"

class CubePrefab : public Prefab
{
public:
	virtual ~CubePrefab() = default;

	void Instantiate(GameObject& gameObject) const override
	{
		gameObject.AddComponent<MeshRenderer>();
	}
};

#endif
