#ifndef SKYDOME_H
#define SKYDOME_H

#include "ScriptComponent.h"

class SkyDome : public ScriptComponent
{
public:
	SkyDome() = default;
	virtual ~SkyDome() = default;

	void Start() override;
	void Update() override;
};

#endif
