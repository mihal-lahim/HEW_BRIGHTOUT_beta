
#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Component.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include <memory>
#include "Material.h"
#include <variant>


// マテリアルバリアント定義
using MaterialVariant3D = std::variant<Material3D*>;



class MeshRenderer : public Component
{
public:
	virtual ~MeshRenderer() = default;

	// メッシュ
	Mesh* mesh = nullptr;
	// マテリアル
	MaterialVariant3D material = {};
};


#endif
