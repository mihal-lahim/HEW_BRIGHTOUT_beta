#include "RenderingSystem.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "direct3d.h"

void RenderingSystem::Initialize()
{
	m_Cameras.clear();
	m_MeshRenderers.clear();
}

void RenderingSystem::Finalize()
{
    m_Cameras.clear();
	m_MeshRenderers.clear();
}

void RenderingSystem::Render()
{
    Direct3D_Clear();
    SetViewport(0);

	Direct3D_SetDepthTest(true);

    // 優先度順に描画
    for (auto& [priority, cameras] : m_Cameras)
    {
        // 同一優先度のカメラごとに描画
        for (auto* camera : cameras)
        {
            if (camera->IsActive() == false) continue;

            // カメラ行列を設定
            camera->SetMatrix();

            // メッシュレンダラーを描画
            for (auto* meshrenderer : m_MeshRenderers)
            {
                if (meshrenderer->IsActive())
                    meshrenderer->Render();
            }
        }
    }

    Direct3D_Present();
}

void RenderingSystem::RegisterCamera(Camera* camera)
{
	// カメラを優先度別に登録
	m_Cameras[camera->Priority].push_back(camera);
}

void RenderingSystem::RegisterMeshRenderer(MeshRenderer* meshRenderer)
{
    // メッシュレンダラーを登録
	m_MeshRenderers.push_back(meshRenderer);
}

void RenderingSystem::UnregisterCamera(Camera* camera)
{
    // 優先度からカメラの配列を取得
	auto& vec = m_Cameras.at(camera->Priority);

    // カメラの配列から目標のカメラを取得
	auto it = std::find(vec.begin(), vec.end(), camera);

    if (it == vec.end()) return;

    // 入れ替えてから破棄
    std::swap(*it, vec.back());
    vec.pop_back();
}

void RenderingSystem::UnregisterMeshRenderer(MeshRenderer* meshRenderer)
{
    // 目標のメッシュレンダラーを取得
    auto it = std::find(m_MeshRenderers.begin(), m_MeshRenderers.end(), meshRenderer);

    if (it == m_MeshRenderers.end()) return;

    // 入れ替えてから破棄
    std::swap(*it, m_MeshRenderers.back());
    m_MeshRenderers.pop_back();
}
