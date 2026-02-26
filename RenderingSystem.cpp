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
#include "UIQuad.h"
#include "BillboardQuad.h"

using namespace DirectX;

void RenderingSystem::Initialize(GraphicsDevice& graphicsDevice)
{
    m_graphicsDevice = &graphicsDevice;

    m_perFrameBuffer.CreateBuffer(*m_graphicsDevice, sizeof(PerFrameConstants));
    m_perCameraBuffer.CreateBuffer(*m_graphicsDevice, sizeof(PerCameraConstants));
    m_perObjectBuffer.CreateBuffer(*m_graphicsDevice, sizeof(PerObjectConstants));

    // ★ UI用 b4
    m_uiParamsBuffer.CreateBuffer(*m_graphicsDevice, sizeof(UIParamsConstants));
}

void RenderingSystem::Finalize()
{
}

void RenderingSystem::Render(const Scene& scene)
{
    if (!m_graphicsDevice)
        return;

    m_graphicsDevice->Clear();

    UpdatePerFrame();

    // -------------------------
    // アニメーション更新
    // -------------------------
    auto animControllers = scene.GetComponents<Animator>();
    for (auto& controller : animControllers)
    {
        if (!controller->IsEnable())
            continue;

        controller->UpdateAnimation();
        controller->Bind(*m_graphicsDevice);
    }

    // -------------------------
    // 3D描画
    // -------------------------
    auto cameras = scene.GetComponents<Camera>();

    std::sort(cameras.begin(), cameras.end(),
        [](const Camera* a, const Camera* b)
        {
            return a->Priority < b->Priority;
        });

    for (auto& camera : cameras)
    {
        if (!camera->IsEnable())
            continue;

        UpdatePerCamera(*camera);

        auto meshRenderers = scene.GetComponents<MeshRenderer>();

        for (auto& renderer : meshRenderers)
        {
            if (!renderer->IsEnable())
                continue;

            if (renderer->renderQueue == RenderQueue::UI)
                continue;

            ApplyRenderQueue(renderer->renderQueue);
            MaterialLoadingProcess(renderer->material);
            renderer->Render(*m_graphicsDevice, m_perObjectBuffer);
        }

        auto skinnedMeshRenderers = scene.GetComponents<SkinnedMeshRenderer>();

        for (auto& renderer : skinnedMeshRenderers)
        {
            if (!renderer->IsEnable())
                continue;

            ApplyRenderQueue(renderer->renderQueue);
            MaterialLoadingProcess(renderer->material);
            renderer->Render(*m_graphicsDevice, m_perObjectBuffer);
        }
    }

    // =============================
    // UI描画（正射影 + b4設定）
    // =============================

    float width = (float)m_graphicsDevice->GetBackBufferWidth();
    float height = (float)m_graphicsDevice->GetBackBufferHeight();

    PerCameraConstants perCamera = {};

    XMMATRIX view = XMMatrixIdentity();
    XMMATRIX projection = XMMatrixOrthographicOffCenterLH(
        0.0f, width,
        height, 0.0f,
        0.0f, 1.0f);

    XMStoreFloat4x4(&perCamera.view, XMMatrixTranspose(view));
    XMStoreFloat4x4(&perCamera.projection, XMMatrixTranspose(projection));

    m_perCameraBuffer.UpdateBuffer(*m_graphicsDevice, &perCamera, sizeof(perCamera));
    m_perCameraBuffer.BindVS(*m_graphicsDevice, 1);
    m_perCameraBuffer.BindPS(*m_graphicsDevice, 1);

    // ★ UiVS の b4 に対応
    UIParamsConstants uiParams{};
    uiParams.ScreenWidth = width;
    uiParams.ScreenHeight = height;
    uiParams.UseScreenSpace = 1.0f;

    m_uiParamsBuffer.UpdateBuffer(*m_graphicsDevice, &uiParams, sizeof(uiParams));
    m_uiParamsBuffer.BindVS(*m_graphicsDevice, 4);
    m_uiParamsBuffer.BindPS(*m_graphicsDevice, 4);

    auto meshRenderers = scene.GetComponents<MeshRenderer>();

    for (auto& renderer : meshRenderers)
    {
        if (!renderer->IsEnable())
            continue;

        if (renderer->renderQueue != RenderQueue::UI)
            continue;

        ApplyRenderQueue(RenderQueue::UI);
        MaterialLoadingProcess(renderer->material);
        renderer->Render(*m_graphicsDevice, m_perObjectBuffer);
    }

    m_graphicsDevice->Present();
}

void RenderingSystem::MaterialLoadingProcess(Material& material)
{
    if (!material.shaderProgram)
    {
        auto* resourceSystem = m_engineCore->GetGameContext().resourceSystem;
        auto* shader = resourceSystem->Load<ShaderProgram>(material.vsPath, material.psPath);
        material.CreateBuffer(*m_graphicsDevice, shader);
    }

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
    PerCameraConstants perCamera = {};

    XMMATRIX view = camera.GetViewMatrix();
    XMMATRIX projection = camera.GetProjectionMatrix(
        (float)m_graphicsDevice->GetBackBufferWidth(),
        (float)m_graphicsDevice->GetBackBufferHeight());

    XMStoreFloat4x4(&perCamera.view, XMMatrixTranspose(view));
    XMStoreFloat4x4(&perCamera.projection, XMMatrixTranspose(projection));

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

    case RenderQueue::UI:
        m_graphicsDevice->SetAlphaBlend(GraphicsDevice::BLEND_TRANSPARENT);
        m_graphicsDevice->SetDepthTest(false);
        break;

    case RenderQueue::Opaque:
    default:
        m_graphicsDevice->SetAlphaBlend(GraphicsDevice::BLEND_OPAQUE);
        m_graphicsDevice->SetDepthTest(true);
        break;
    }
}

std::shared_ptr<Mesh> RenderingSystem::CreateUIQuad()
{
    if (!m_uiQuad)
        m_uiQuad = ::CreateUIQuad(*m_graphicsDevice);

    return m_uiQuad;
}

std::shared_ptr<Mesh> RenderingSystem::CreateUIQuadWithUV(float u0, float u1)
{
    return ::CreateUIQuadWithUV(*m_graphicsDevice, u0, u1);
}

std::shared_ptr<Mesh> RenderingSystem::CreateBillboardQuad()
{
    if (!m_billboardQuad)
        m_billboardQuad = ::CreateBillboardQuad(*m_graphicsDevice);

    return m_billboardQuad;
}