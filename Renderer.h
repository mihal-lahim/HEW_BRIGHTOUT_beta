
#ifndef RENDERER_H
#define RENDERER_H

#include "Component.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "StructuredBuffer.h"
#include <DirectXMath.h>
#include "Transform.h"
#include <memory>
#include "Material.h"
#include "Model.h"

enum class RenderQueue
{
	Opaque = 0,
	Transparent = 1
};

enum class RendererType
{
	Mesh,
	Skinned
};

class Renderer : public Component
{
public:
	virtual void Render(GraphicsDevice& device) = 0;
	virtual RendererType GetRendererType() const = 0;
	RenderQueue renderQueue = RenderQueue::Opaque;
	Material material = {};
};

class MeshRenderer : public Renderer
{
public:
	// ï`âÊèàóù
	void Render(GraphicsDevice& device) override;
	RendererType GetRendererType() const override { return RendererType::Mesh; }

	const Mesh* mesh = nullptr;
};

class SkinnedMeshRenderer : public Renderer
{
public:
	// ï`âÊèàóù
	void Render(GraphicsDevice& device) override;
	RendererType GetRendererType() const override { return RendererType::Skinned; }

	const SkinnedMesh* mesh = nullptr;
	AnimationController* animationController = nullptr;
	const Skeleton* skeleton = nullptr;
	StructuredBuffer boneBuffer = {};
	UINT boneBufferSize = 0;

	void UpdateBoneBuffer(GraphicsDevice& device, const DirectX::XMMATRIX* matrices, size_t count)
	{
		if (boneBufferSize != count)
		{
			boneBuffer.CreateBuffer(device, static_cast<UINT>(count), sizeof(DirectX::XMMATRIX), USAGE_TYPE::DYNAMIC, VIEW_TYPE::SRV);
			boneBufferSize = static_cast<UINT>(count);
		}

		boneBuffer.UpdateBuffer(device, matrices);
	}

	void BindBoneBuffer(GraphicsDevice& device, UINT slot)
	{
		boneBuffer.BindVS(device, slot);
	}
};


#endif
