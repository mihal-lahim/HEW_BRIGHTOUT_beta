#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "ScriptComponent.h"
#include "Mesh.h"
#include <memory>

class Billboard : public ScriptComponent
{
public:
	void Awake() override;
	void Update() override;
private:
	GameObject* m_mainCamera = nullptr;
	std::shared_ptr<Mesh> m_quadMesh;
};

#endif
