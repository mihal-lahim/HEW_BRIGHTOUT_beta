#include "RenderingSystem.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "GraphicsDevice.h"
#include "EngineCore.h"
#include "shader3d.h"
#include "Scene.h"
#include <algorithm>

using namespace DirectX;

void RenderingSystem::SetDevice(GraphicsDevice* device)
{
	m_graphicsDevice = device;
}

void RenderingSystem::Initialize()
{
}

void RenderingSystem::Finalize()
{
}

void RenderingSystem::Render(const Scene& scene)
{
	GetGraphicsDevice().Clear();
	GetGraphicsDevice().SetViewport(0);

	GetGraphicsDevice().SetDepthTest(true);

	// シーンからカメラとメッシュレンダラーを取得
	auto cameras = scene.GetComponents<Camera>();
	auto meshRenderers = scene.GetComponents<MeshRenderer>();

	// カメラを優先度順にソート
    std::stable_sort(cameras.begin(), cameras.end(),
        [](Camera* a, Camera* b) { return a->Priority > b->Priority; });

    // 優先度順に描画
    for (auto& camera : cameras)
    {
        if (camera->IsEnable() == false) continue;

        // カメラ行列を設定
		XMMATRIX view = camera->GetViewMatrix();
		XMMATRIX projection = camera->GetProjectionMatrix();

		Shader3d_SetViewMatrix(view);
		Shader3d_SetProjectionMatrix(projection);

		GetGraphicsDevice().SetAlphaBlend(GraphicsDevice::BLEND_OPAQUE);

        // メッシュレンダラーを描画
        for (auto* meshRenderer : meshRenderers)
        {
            if (meshRenderer->IsEnable())
                meshRenderer->Render(*m_graphicsDevice);
        }

		GetGraphicsDevice().SetAlphaBlend(GraphicsDevice::BLEND_TRANSPARENT);
    }

	GetGraphicsDevice().Present();
}
