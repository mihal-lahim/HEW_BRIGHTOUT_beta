#include "RenderingSystem.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "SpriteRenderer.h"
#include "GraphicsDevice.h"
#include "EngineCore.h"
#include "Material.h"
#include "ResourceSystem.h"
#include "Shader.h"
#include "shader3d.h"
#include "Scene.h"
#include <algorithm>
#include "MaptipManager.h"

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

	auto& gameContext = m_engineCore->GetGameContext();
	auto* resourceSystem = gameContext.resourceSystem;
	if (!resourceSystem)
	{
		return;
	}

	if (!m_defaultVertexShader)
	{
		m_defaultVertexShader = resourceSystem->Load<MeshVS>();
	}
	if (!m_defaultPixelShader)
	{
		m_defaultPixelShader = resourceSystem->Load<MeshPS>();
	}
	if (!m_defaultMaterialInstance && m_defaultPixelShader)
	{
		Material3D defaultMaterial{};
		defaultMaterial.PixelShader = m_defaultPixelShader;
		m_defaultMaterialInstance.reset(defaultMaterial.Instantiate());
	}

	PerFrameCB::CBData perFrameData{};
	perFrameData.AmbientColor = { 0.4f, 0.4f, 0.4f, 1.0f };
	perFrameData.DirectionalColor = { 0.5f, 0.5f, 0.5f, 1.0f };
	perFrameData.DirectionalVec = { -0.3f, -1.0f, 0.2f };
	if (m_defaultPixelShader)
	{
		m_defaultPixelShader->UpdatePerFrameCB(GetGraphicsDevice(), perFrameData);
	}

	// シーンからカメラとメッシュレンダラーを取得
	auto cameras = scene.GetComponents<Camera>();
	auto meshRenderers = scene.GetComponents<MeshRenderer>();
	auto spriteRenderers = scene.GetComponents<SpriteRenderer>();

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


		MapTipManager::Instance().RenderAll();
 
        // メッシュレンダラーを描画
		PerCameraCB::CBData cameraData{};
		XMStoreFloat4x4(&cameraData.ViewMatrix, XMMatrixTranspose(view));
		XMStoreFloat4x4(&cameraData.ProjectionMatrix, XMMatrixTranspose(projection));
		auto& cameraPosition = camera->gameObject().transform().position();
		cameraData.CameraPosition = { cameraPosition.x, cameraPosition.y, cameraPosition.z };

		for (auto* meshRenderer : meshRenderers)
		{
			if (!meshRenderer->IsEnable())
			{
				continue;
			}

			auto* vertexShader = meshRenderer->GetVertexShader();
			if (!vertexShader)
			{
				vertexShader = m_defaultVertexShader;
				meshRenderer->SetVertexShader(vertexShader);
			}
			if (vertexShader)
			{
				vertexShader->UpdatePerCameraCB(GetGraphicsDevice(), cameraData);
			}

			auto* materialInstance = meshRenderer->GetMaterialInstance();
			if (!materialInstance && m_defaultMaterialInstance)
			{
				materialInstance = m_defaultMaterialInstance.get();
				meshRenderer->SetMaterialInstance(materialInstance);
			}
			if (materialInstance && materialInstance->PixelShader)
			{
				materialInstance->PixelShader->UpdatePerFrameCB(GetGraphicsDevice(), perFrameData);
			}

			meshRenderer->Render(*m_graphicsDevice);
		}

		GetGraphicsDevice().SetAlphaBlend(GraphicsDevice::BLEND_TRANSPARENT);
    }

	if (!spriteRenderers.empty())
	{
		GetGraphicsDevice().SetDepthTest(false);
		GetGraphicsDevice().SetAlphaBlend(GraphicsDevice::BLEND_TRANSPARENT);

		PerCameraCB::CBData spriteCameraData{};
		float width = static_cast<float>(GetGraphicsDevice().GetBackBufferWidth());
		float height = static_cast<float>(GetGraphicsDevice().GetBackBufferHeight());
		auto projection2D = XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&spriteCameraData.ViewMatrix, XMMatrixTranspose(projection2D));
		XMStoreFloat4x4(&spriteCameraData.ProjectionMatrix, XMMatrixIdentity());
		spriteCameraData.CameraPosition = { 0.0f, 0.0f, 0.0f };

		for (auto* spriteRenderer : spriteRenderers)
		{
			if (!spriteRenderer->IsEnable())
			{
				continue;
			}

			auto* vertexShader = spriteRenderer->GetVertexShader();
			if (!vertexShader)
			{
				vertexShader = m_defaultSpriteVertexShader;
				spriteRenderer->SetVertexShader(vertexShader);
			}
			if (vertexShader)
			{
				vertexShader->UpdatePerCameraCB(GetGraphicsDevice(), spriteCameraData);
			}

			auto* materialInstance = spriteRenderer->GetMaterialInstance();
			if (!materialInstance && m_defaultSpriteMaterialInstance)
			{
				materialInstance = m_defaultSpriteMaterialInstance.get();
				spriteRenderer->SetMaterialInstance(materialInstance);
			}

			spriteRenderer->Render(*m_graphicsDevice);
		}
	}

	GetGraphicsDevice().Present();
}