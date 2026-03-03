#include "Billboard.h"
#include <DirectXMath.h>
#include "GameObject.h"
#include "Renderer.h"
#include "RenderingSystem.h"

using namespace DirectX;

namespace
{
	void ApplyBillboardShaderRecursive(GameObject* gameObject, float rotation, const std::string& vsPath)
	{
		if (!gameObject) return;

		auto meshRenderers = gameObject->GetComponents<MeshRenderer>();
		for (auto* renderer : meshRenderers)
		{
			renderer->material.vsPath = vsPath;
			renderer->material.psPath = "BillboardPS.cso";
			renderer->material.shaderProgram = nullptr;
			renderer->renderQueue = RenderQueue::Transparent;
			renderer->material.SetFloat4("uv_rect", { 0.0f, 0.0f, 1.0f, 1.0f });
			renderer->material.SetFloat("uv_rotation", rotation);
		}

		for (auto* child : gameObject->GetChildren())
		{
			ApplyBillboardShaderRecursive(child, rotation, vsPath);
		}
	}
}

void Billboard::Awake()
{
	m_mainCamera = GetGameObjectByTag("MainCamera");

	std::string vsPath = customVsPath.empty() ? "BillboardVS.cso" : customVsPath;

	// Quadメッシュを取得（キャッシュ済み）
	m_quadMesh = gameObject().rendering().CreateBillboardQuad();

	// 既存の MeshRenderer があればシェーダーとメッシュを差し替え
	auto* renderer = gameObject().GetComponent<MeshRenderer>();
	if (renderer)
	{
		renderer->mesh = m_quadMesh.get();
		renderer->material.vsPath = vsPath;
		renderer->material.psPath = "BillboardPS.cso";
		renderer->material.shaderProgram = nullptr;
		renderer->renderQueue = RenderQueue::Transparent;
		renderer->material.SetFloat4("uv_rect", { 0.0f, 0.0f, 1.0f, 1.0f });
		renderer->material.SetFloat("uv_rotation", uvRotation);
	}
	else
	{
		// MeshRenderer が無い場合は自動で追加
		renderer = gameObject().AddComponent<MeshRenderer>();
		renderer->mesh = m_quadMesh.get();
		renderer->material.vsPath = vsPath;
		renderer->material.psPath = "BillboardPS.cso";
		renderer->material.shaderProgram = nullptr;
		renderer->renderQueue = RenderQueue::Transparent;
		renderer->material.SetFloat4("uv_rect", { 0.0f, 0.0f, 1.0f, 1.0f });
		renderer->material.SetFloat("uv_rotation", uvRotation);
	}
}

void Billboard::Update()
{
	// m_mainCamera が既にキャッシュされていればタグ検索をスキップ
	if (m_mainCamera && m_mainCamera->IsActiveInHierarchy())
	{
		return;
	}

	m_mainCamera = GetGameObjectByTag("MainCamera");
}
