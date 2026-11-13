#include "SkyDome.h"
#include "GameObject.h"
#include "Scene.h"
#include "Renderer.h"

using namespace DirectX;

// 再帰的に全子孫の MeshRenderer を取得するヘルパー関数
static void CollectMeshRenderers(GameObject* obj, std::vector<MeshRenderer*>& out)
{
	auto renderers = obj->GetComponents<MeshRenderer>();
	for (auto* r : renderers)
	{
		out.push_back(r);
	}

	auto children = obj->GetChildren();
	for (auto* child : children)
	{
		CollectMeshRenderers(child, out);
	}
}

void SkyDome::Start()
{
	// スカイドーム配下の全 MeshRenderer を再帰的に収集
	std::vector<MeshRenderer*> allRenderers;
	CollectMeshRenderers(&gameObject(), allRenderers);

	OutputDebugStringA(("[SkyDome] Found " + std::to_string(allRenderers.size()) + " MeshRenderer(s)\n").c_str());

	for (auto* r : allRenderers)
	{
		r->renderQueue = RenderQueue::Background;
		r->material.psPath = "SkyPS.cso";
		r->material.shaderProgram = nullptr; // シェーダー再読み込み
		OutputDebugStringA("[SkyDome] Applied SkyPS.cso to renderer\n");
	}
}

void SkyDome::Update()
{
}
