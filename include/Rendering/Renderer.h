#ifndef RENDERER_H
#define RENDERER_H

#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include <memory>
#include "Material.h"
#include "Model.h"
#include "ConstantBuffer.h"

enum class RenderQueue
{
	Background = -1,
	Opaque = 0,
	Transparent = 1,
	UI = 2
};

enum class RendererType
{
	Mesh,
	SkinnedMesh
};

class Renderer : public Component
{
public:
	virtual void Render(GraphicsDevice& device, ConstantBuffer& perObject) = 0;
	RenderQueue renderQueue = RenderQueue::Opaque;
	RendererType rendererType = RendererType::Mesh;
	Material material = {};
};

class MeshRenderer : public Renderer
{
public:
	MeshRenderer()
	{
		rendererType = RendererType::Mesh;
	}

	// •`‰æˆ—
	void Render(GraphicsDevice& device, ConstantBuffer& perObject) override;

	const Model* model = nullptr;
	const Mesh* mesh = nullptr;
};

class SkinnedMeshRenderer : public Renderer
{
public:
	SkinnedMeshRenderer()
	{
		rendererType = RendererType::SkinnedMesh;
	}
	// •`‰æˆ—
	void Render(GraphicsDevice& device, ConstantBuffer& perObject) override;

	const Model* model = nullptr;
	const SkinnedMesh* skinnedMesh = nullptr;
};


#endif
