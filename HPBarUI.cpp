#include "HPBarUI.h"
#include "SceneSystem.h"
#include "Renderer.h"
#include "UIQuad.h"
#include "Mesh.h"
#include "RenderingSystem.h"
#include "Health.h"
#include <algorithm>

using namespace std;

void HPBarUI::Start()
{
	// シーン / レンダリング / グラフィックスデバイス取得
	auto* scene = gameObject().scenePtr();
	auto& rendering = gameObject().rendering();
	GraphicsDevice& device = rendering.GetGraphicsDevice();

	// 元スケール記録
	m_originalScale = scale;

	// 背景オブジェクト（空フレーム）
	m_bgObj = scene->CreateGameObject();
	m_bgObj->SetName("HPBar_Background");
	m_bgObj->transform().position() = position;
	m_bgObj->transform().scale() = scale;

	m_bgRenderer = m_bgObj->AddComponent<MeshRenderer>();
	m_bgRenderer->mesh = rendering.CreateUIQuad().get();
	m_bgRenderer->renderQueue = RenderQueue::UI;
	m_bgRenderer->material.texturePath = backgroundTexture;
	m_bgRenderer->material.vsPath = vsPath;
	m_bgRenderer->material.psPath = psPath;

	// 前景オブジェクト（ゲージ） - 初期はフル
	m_fillObj = scene->CreateGameObject();
	m_fillObj->SetName("HPBar_Fill");
	m_fillObj->transform().position() = position;
	m_fillObj->transform().scale() = scale;

	m_fillRenderer = m_fillObj->AddComponent<MeshRenderer>();
	// フル幅のメッシュをひとつ生成しておく
	m_meshCache[100] = CreateUIQuadWithUV(device, 0.0f, 1.0f);
	m_fillRenderer->mesh = m_meshCache[100].get();
	m_fillRenderer->renderQueue = RenderQueue::UI;
	m_fillRenderer->material.texturePath = fillTexture;
	m_fillRenderer->material.vsPath = vsPath;
	m_fillRenderer->material.psPath = psPath;

	// Health コンポーネントを探す（シーン内の最初の Health を利用）
	auto healths = scene->GetComponents<Health>();
	if (!healths.empty())
	{
		m_playerHealth = healths.front();
	}
	// 初回パーセントをセット
	m_lastPercent = -1;
}

void HPBarUI::Update()
{
	// Health が見つからない場合は何もしない
	if (!m_playerHealth || !m_fillRenderer || !m_fillObj)
		return;

	// 割合を計算
	float cur = m_playerHealth->GetCurrentHealth();
	float max = m_playerHealth->GetMaxHealth();
	float percent = (max > 0.0f) ? std::clamp(cur / max, 0.0f, 1.0f) : 0.0f;

	// 整数パーセンテージ（0..100）
	int p = static_cast<int>(std::lround(percent * 100.0f));
	if (p < 0) p = 0;
	if (p > 100) p = 100;

	// 前回と同じなら更新しない（メッシュ再生成コストを抑える）
	if (p == m_lastPercent)
		return;

	m_lastPercent = p;

	// グラフィックスデバイス取得（メッシュ生成に必要）
	auto& rendering = gameObject().rendering();
	GraphicsDevice& device = rendering.GetGraphicsDevice();

	// UV 範囲を 0..percent にしてメッシュを生成またはキャッシュから取得
	float u1 = percent; // 0.0 .. 1.0
	if (u1 < 0.0f) u1 = 0.0f;
	if (u1 > 1.0f) u1 = 1.0f;

	// キャッシュ参照
	auto it = m_meshCache.find(p);
	if (it == m_meshCache.end())
	{
		// CreateUIQuadWithUV は device と u0,u1 を渡す
		m_meshCache[p] = CreateUIQuadWithUV(device, 0.0f, u1);
		it = m_meshCache.find(p);
	}

	if (it != m_meshCache.end())
	{
		m_fillRenderer->mesh = it->second.get();
	}

	// スケールは元スケールの X を percent 倍して、幅を縮める（左揃え）
	Vector3 newScale = m_originalScale;
	newScale.x = m_originalScale.x * percent;
	m_fillObj->transform().scale() = newScale;
}