
#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Component.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include <memory>
#include "Material.h"



class MeshRenderer : public Component
{
public:
	virtual ~MeshRenderer() = default;
};


#endif
