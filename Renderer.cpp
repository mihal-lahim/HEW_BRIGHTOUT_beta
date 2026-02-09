#include "Renderer.h"
#include "Texture.h"
#include "ResourceSystem.h"
#include "GameObject.h"
#include "DebugOstream.h"

void MeshRenderer::Render(GraphicsDevice& device)
{
	if (!mesh)
	{
		return;
	}

	mesh->Draw(device);
}

void SkinnedMeshRenderer::Render(GraphicsDevice& device)
{
	if (!mesh)
	{
		return;
	}

	mesh->Draw(device);
}
