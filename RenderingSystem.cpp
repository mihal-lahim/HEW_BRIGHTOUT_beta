#include "RenderingSystem.h"
#include "Animation.h"
#include "Camera.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "EngineCore.h"
#include "Material.h"
#include "ResourceSystem.h"
#include "Shader.h"
#include "Scene.h"
#include "Texture.h"
#include <algorithm>
#include <vector>

using namespace DirectX;

void RenderingSystem::Initialize(GraphicsDevice& graphicsDevice)
{
	m_graphicsDevice = &graphicsDevice;
	if (m_buffersInitialized)
	{
		return;
	}

	m_perFrameBuffer.CreateBuffer(*m_graphicsDevice, sizeof(PerFrameConstants));
	m_perCameraBuffer.CreateBuffer(*m_graphicsDevice, sizeof(PerCameraConstants));
	m_perObjectBuffer.CreateBuffer(*m_graphicsDevice, sizeof(PerObjectConstants));
	m_buffersInitialized = true;
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

	if (!m_defaultTexture)
	{
		m_defaultTexture = scene.resource().Load<Texture>(L"texture/Default.png");
	}
	if (!m_defaultMeshShader)
	{
		m_defaultMeshShader = scene.resource().Load<ShaderProgram>("MeshVS.cso", "MeshPS.cso");
	}
	if (!m_defaultSkinnedMeshShader)
	{
		m_defaultSkinnedMeshShader = scene.resource().Load<ShaderProgram>("SkinnedMeshVS.cso", "MeshPS.cso");
	}

	const auto animationControllers = scene.GetComponents<AnimationController>();
	for (auto* controller : animationControllers)
	{
		if (controller && controller->IsEnable())
		{
			controller->Update();
		}
	}

	UpdatePerFrame();
	UpdatePerCamera(scene);

	std::vector<Renderer*> renderers = {};
	const auto meshRenderers = scene.GetComponents<MeshRenderer>();
	const auto skinnedMeshRenderers = scene.GetComponents<SkinnedMeshRenderer>();
	for (auto* renderer : meshRenderers)
	{
		if (renderer && renderer->IsEnable() && renderer->mesh)
		{
			renderers.push_back(renderer);
		}
	}
	for (auto* renderer : skinnedMeshRenderers)
	{
		if (renderer && renderer->IsEnable() && renderer->mesh)
		{
			renderers.push_back(renderer);
		}
	}

	std::sort(renderers.begin(), renderers.end(), [](const Renderer* a, const Renderer* b)
		{
			return static_cast<int>(a->renderQueue) < static_cast<int>(b->renderQueue);
		});

	RenderQueue currentQueue = RenderQueue::Opaque;
	ApplyRenderQueue(currentQueue);
	for (auto* renderer : renderers)
	{
		if (!renderer)
		{
			continue;
		}

		if (renderer->renderQueue != currentQueue)
		{
			currentQueue = renderer->renderQueue;
			ApplyRenderQueue(currentQueue);
		}

		const RendererType rendererType = renderer->rendererType;
		if (!renderer->material.shaderProgram)
		{
			renderer->material.shaderProgram = rendererType == RendererType::Mesh ? m_defaultMeshShader : m_defaultSkinnedMeshShader;
		}
		if (!renderer->material.IsInitialized() && renderer->material.shaderProgram)
		{
			renderer->material.CreateBuffer(*m_graphicsDevice, renderer->material.shaderProgram);
		}
		if (!renderer->material.texture)
		{
			renderer->material.texture = m_defaultTexture;
		}
		if (renderer->material.shaderProgram)
		{
			renderer->material.shaderProgram->Bind(*m_graphicsDevice);
		}
		if (renderer->material.texture)
		{
			renderer->material.texture->Bind(*m_graphicsDevice);
		}
		renderer->material.Apply(*m_graphicsDevice);
		renderer->material.Bind(*m_graphicsDevice);

		switch (rendererType)
		{
		case RendererType::Mesh:
			RenderMeshRenderer(*static_cast<MeshRenderer*>(renderer));
			break;
		case RendererType::SkinnedMesh:
			RenderSkinnedMeshRenderer(*static_cast<SkinnedMeshRenderer*>(renderer));
			break;
		}
	}

	m_graphicsDevice->Present();
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

void RenderingSystem::UpdatePerCamera(const Scene& scene)
{
	PerCameraConstants perCamera = {};
	XMMATRIX view = XMMatrixIdentity();
	XMMATRIX projection = XMMatrixIdentity();
	const auto cameras = scene.GetComponents<Camera>();
	if (!cameras.empty())
	{
		auto* camera = *std::max_element(cameras.begin(), cameras.end(), [](const Camera* a, const Camera* b)
			{
				return a->Priority < b->Priority;
			});
		if (camera)
		{
			view = camera->GetViewMatrix();
			projection = camera->GetProjectionMatrix(static_cast<float>(m_graphicsDevice->GetBackBufferWidth()),
				static_cast<float>(m_graphicsDevice->GetBackBufferHeight()));
		}
	}

	XMStoreFloat4x4(&perCamera.view, XMMatrixTranspose(view));
	XMStoreFloat4x4(&perCamera.projection, XMMatrixTranspose(projection));

	m_perCameraBuffer.UpdateBuffer(*m_graphicsDevice, &perCamera, sizeof(perCamera));
	m_perCameraBuffer.BindVS(*m_graphicsDevice, 1);
	m_perCameraBuffer.BindPS(*m_graphicsDevice, 1);
}

void RenderingSystem::UpdatePerObject(const Transform& transform)
{
	PerObjectConstants perObject = {};
	XMStoreFloat4x4(&perObject.world, XMMatrixTranspose(transform.GetWorldMatrix()));

	m_perObjectBuffer.UpdateBuffer(*m_graphicsDevice, &perObject, sizeof(perObject));
	m_perObjectBuffer.BindVS(*m_graphicsDevice, 2);
	m_perObjectBuffer.BindPS(*m_graphicsDevice, 2);
}

void RenderingSystem::RenderMeshRenderer(MeshRenderer& renderer)
{
	UpdatePerObject(renderer.gameObject().transform());
	renderer.Render(*m_graphicsDevice);
}

void RenderingSystem::RenderSkinnedMeshRenderer(SkinnedMeshRenderer& renderer)
{
	UpdatePerObject(renderer.gameObject().transform());

	if (renderer.animationController && renderer.skeleton)
	{
		const auto& boneTransforms = renderer.animationController->GetBoneTransforms();
		const auto& bones = renderer.skeleton->bones;
		const size_t boneCount = std::min(boneTransforms.size(), bones.size());
		if (boneCount > 0)
		{
			std::vector<XMMATRIX> matrices(boneCount, XMMatrixIdentity());
			for (size_t i = 0; i < boneCount; ++i)
			{
				const Transform* boneTransform = boneTransforms[i];
				if (boneTransform)
				{
					const XMMATRIX boneMatrix =
						boneTransform->GetLocalMatrix() * bones[i].offsetMatrix;
					matrices[i] = XMMatrixTranspose(boneMatrix);
				}
			}

			renderer.UpdateBoneBuffer(*m_graphicsDevice, matrices.data(), boneCount);
			renderer.BindBoneBuffer(*m_graphicsDevice, 0);
		}
	}

	renderer.Render(*m_graphicsDevice);
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
