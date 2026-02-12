#include "Renderer.h"
#include "Texture.h"
#include "GameObject.h"
#include "DebugOstream.h"
#include "RenderingSystem.h"

using namespace DirectX;

void MeshRenderer::Render(GraphicsDevice& device, ConstantBuffer& perObject)
{
	if (!mesh)
	{
		return;
	}

	// オブジェクト毎の定数バッファの更新とバインド
	RenderingSystem::PerObjectConstants perObjectData = {};
	XMStoreFloat4x4(&perObjectData.world, XMMatrixTranspose(gameObject().transform().GetWorldMatrix()));
	perObject.UpdateBuffer(device, &perObjectData, sizeof(RenderingSystem::PerObjectConstants));
	perObject.BindVS(device, 2);
	perObject.BindPS(device, 2);

	// マテリアルの適用とバインド
	material.Apply(device);
	material.Bind(device);

	// メッシュの描画
	mesh->Draw(device);
}

void SkinnedMeshRenderer::Render(GraphicsDevice& device, ConstantBuffer& perObject)
{
	if (!skinnedMesh)
	{
		return;
	}
	// オブジェクト毎の定数バッファの更新とバインド
	RenderingSystem::PerObjectConstants perObjectData = {};
	XMStoreFloat4x4(&perObjectData.world, XMMatrixTranspose(gameObject().transform().GetWorldMatrix()));
	perObject.UpdateBuffer(device, &perObjectData, sizeof(RenderingSystem::PerObjectConstants));
	perObject.BindVS(device, 2);
	perObject.BindPS(device, 2);

	// マテリアルの適用とバインド
	material.Apply(device);
	material.Bind(device);

	// メッシュの描画
	skinnedMesh->Draw(device);
}
