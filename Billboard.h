#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "ScriptComponent.h"

class Billboard : public ScriptComponent
{
public:
	void Awake() override;
	void Update() override;
private:
	GameObject* m_mainCamera = nullptr;
};

#endif
