#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "ScriptComponent.h"
#include "Mesh.h"
#include <memory>
#include <string>

class Billboard : public ScriptComponent
{
public:
	float uvRotation = 0.0f;
	std::string customVsPath;

	void Awake() override;
	void Update() override;
private:
	GameObject* m_mainCamera = nullptr;
	std::shared_ptr<Mesh> m_quadMesh;
};

#endif
