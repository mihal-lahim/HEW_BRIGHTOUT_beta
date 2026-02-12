#include "RenderingSystem.h"
#include "Camera.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "EngineCore.h"
#include "Material.h"
#include "ResourceSystem.h"
#include "Shader.h"
#include "Scene.h"
#include "Texture.h"
#include "Animation.h"
#include <algorithm>
#include <vector>

using namespace DirectX;

void RenderingSystem::Initialize(GraphicsDevice& graphicsDevice)
{
	// グラフィックスデバイスの設定
	m_graphicsDevice = &graphicsDevice;

	// 各種定数バッファの作成
	m_perFrameBuffer.CreateBuffer(*m_graphicsDevice, sizeof(PerFrameConstants));
	m_perCameraBuffer.CreateBuffer(*m_graphicsDevice, sizeof(PerCameraConstants));
	m_perObjectBuffer.CreateBuffer(*m_graphicsDevice, sizeof(PerObjectConstants));
}

void RenderingSystem::Finalize()
{
}

void RenderingSystem::Render(const Scene& scene)
{
	if (!m_graphicsDevice)
	{
		return;
	}

	m_graphicsDevice->Clear();

	// フレーム毎の更新
	UpdatePerFrame();

	
	// アニメーションコントローラーの取得
	auto animControllers = scene.GetComponents<Animator>();
	for (auto& controller : animControllers)
	{
		if (!controller->IsEnable())
		{
			continue;
		}
		// アニメーションの更新
		controller->UpdateAnimation();
		controller->Bind(*m_graphicsDevice);
	}
	


	// カメラの取得
	auto cameras = scene.GetComponents<Camera>();

	std::sort(cameras.begin(), cameras.end(),
		[](const Camera* a, const Camera* b)
		{
			return a->Priority < b->Priority;
		}
	);

	for (auto& camera : cameras)
	{
		if (!camera->IsEnable())
		{
			continue;
		}
		// カメラ毎の更新
		UpdatePerCamera(*camera);

		// メッシュレンダラーの取得
		auto meshRenderers = scene.GetComponents<MeshRenderer>();

		for (auto& renderer : meshRenderers)
		{
			if (!renderer->IsEnable())
			{
				continue;
			}
			// レンダーキューの適用
			RenderQueue currentQueue = renderer->renderQueue;
			ApplyRenderQueue(currentQueue);

			// マテリアルの遅延ロード処理
			MaterialLoadingProcess(renderer->material);

			// レンダー
			renderer->Render(*m_graphicsDevice, m_perObjectBuffer);
		}

		// スキンドメッシュレンダラーの取得
		auto skinnedMeshRenderers = scene.GetComponents<SkinnedMeshRenderer>();

		for (auto& renderer : skinnedMeshRenderers)
		{
			if (!renderer->IsEnable())
			{
				continue;
			}
			// レンダーキューの適用
			RenderQueue currentQueue = renderer->renderQueue;
			ApplyRenderQueue(currentQueue);

			// マテリアルの遅延ロード処理
			MaterialLoadingProcess(renderer->material);

			// レンダー
			renderer->Render(*m_graphicsDevice, m_perObjectBuffer);
		}
	}

	m_graphicsDevice->Present();
}

void RenderingSystem::MaterialLoadingProcess(Material& material)
{
	// シェーダープログラムの設定
	if (!material.shaderProgram)
	{
		auto* resourceSystem = m_engineCore->GetGameContext().resourceSystem;
		auto* shader = resourceSystem->Load<ShaderProgram>(material.vsPath, material.psPath);
		material.CreateBuffer(*m_graphicsDevice, shader);
	}
	// テクスチャの設定
	if (!material.texture)
	{
		auto* resourceSystem = m_engineCore->GetGameContext().resourceSystem;
		auto* texture = resourceSystem->Load<Texture>(material.texturePath);
		material.texture = texture;
	}
}

void RenderingSystem::UpdatePerFrame()
{
	PerFrameConstants perFrame = {};
	perFrame.ambient_light_color = { 0.3f, 0.3f, 0.3f, 1.0f };
	perFrame.directional_light_color = { 0.4f, 0.4f, 0.4f, 1.0f };
	perFrame.directional_light_vector = { 0.4f, -1.0f, 2.0f, 0.0f };

	m_perFrameBuffer.UpdateBuffer(*m_graphicsDevice, &perFrame, sizeof(perFrame));
	m_perFrameBuffer.BindVS(*m_graphicsDevice, 0);
	m_perFrameBuffer.BindPS(*m_graphicsDevice, 0);
}

void RenderingSystem::UpdatePerCamera(const Camera& camera)
{
	// カメラ毎構造体の更新
	PerCameraConstants perCamera = {};
	XMMATRIX view = camera.GetViewMatrix();
	XMMATRIX projection = camera.GetProjectionMatrix(
		static_cast<float>(m_graphicsDevice->GetBackBufferWidth()),
		static_cast<float>(m_graphicsDevice->GetBackBufferHeight())
	);

	// 行列を転置して格納
	XMStoreFloat4x4(&perCamera.view, XMMatrixTranspose(view));
	XMStoreFloat4x4(&perCamera.projection, XMMatrixTranspose(projection));

	// 定数バッファの更新とバインド
	m_perCameraBuffer.UpdateBuffer(*m_graphicsDevice, &perCamera, sizeof(perCamera));
	m_perCameraBuffer.BindVS(*m_graphicsDevice, 1);
	m_perCameraBuffer.BindPS(*m_graphicsDevice, 1);
}

void RenderingSystem::ApplyRenderQueue(RenderQueue queue)
{
	switch (queue)
	{
	case RenderQueue::Transparent:
		m_graphicsDevice->SetAlphaBlend(GraphicsDevice::BLEND_TRANSPARENT);
		m_graphicsDevice->SetDepthTest(true);
		break;
	case RenderQueue::Opaque:
	default:
		m_graphicsDevice->SetAlphaBlend(GraphicsDevice::BLEND_OPAQUE);
		m_graphicsDevice->SetDepthTest(true);
		break;
	}
}
