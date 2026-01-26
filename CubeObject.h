
#ifndef CUBE_OBJECT_H
#define CUBE_OBJECT_H

#include "GameObject.h"


class CubeObject : public GameObject
{
public:
	CubeObject() = default;
	virtual ~CubeObject() = default;

	void Awake() override;
};

#endif
