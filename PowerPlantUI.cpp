#include "PowerPlantUI.h"
#include "SceneSystem.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "UIQuad.h"
#include "Mesh.h"
#include "PowerPlant.h"
#include "Health.h"
#include <cmath>

using namespace std;

void PowerPlantUI::Start()
{
	// 親となる PowerPlant コンポーネントを取得
	m_powerPlant = gameObject().GetComponent<PowerPlant>();
	// 描画用デバイス取得
	auto* scene = gameObject().scenePtr();
	auto& rendering = gameObject().rendering();
	GraphicsDevice& device = rendering.GetGraphicsDevice();

	// 元スケールを保持
	m_originalScale = uiScale;

	// 背景オブジェクト
	m_bgObj = scene->CreateGameObject();
	m_bgObj->SetName("PowerPlantUI_BG");
	// ワールド座標：プラントの位置 + localOffset（プラントと同じ高さや向きに配置）
	Vector3 basePos = gameObject().transform().position();
	m_bgObj->transform().position() = basePos + localOffset;
	m_bgObj->transform().scale() = uiScale;

	m_bgRenderer = m_bgObj->AddComponent<MeshRenderer>();
	m_bgRenderer->mesh = gameObject().rendering().CreateUIQuad().get();
	m_bgRenderer->renderQueue = RenderQueue::UI;
	m_bgRenderer->material.texturePath = backgroundTexture;
	m_bgRenderer->material.vsPath = "UiVS.cso";
	m_bgRenderer->material.psPath = "UiPS.cso";

	// フィルオブジェクト（背景の前面に重ねる）
	m_fillObj = scene->CreateGameObject();
	m_fillObj->SetName("PowerPlantUI_Fill");
	m_fillObj->transform().position() = basePos + localOffset;
	m_fillObj->transform().scale() = uiScale;

	m_fillRenderer = m_fillObj->AddComponent<MeshRenderer>();
	// 初期は 0% 伸長（縦方向）
	m_meshCache[0] = CreateUIQuadWithVRange(device, 0.0f, 0.0f);
	m_fillRenderer->mesh = m_meshCache[0].get();
	m_fillRenderer->renderQueue = RenderQueue::UI;
	m_fillRenderer->material.texturePath = fillTexture;
	m_fillRenderer->material.vsPath = "UiVS.cso";
	m_fillRenderer->material.psPath = "UiPS.cso";
}

void PowerPlantUI::Update()
{
	// PowerPlant が無ければ何もしない
	if (!m_powerPlant || !m_fillRenderer || !m_fillObj || !m_bgObj)
		return;

	// UI のワールド位置をプラントに追従させる
	Vector3 basePos = gameObject().transform().position();
	Vector3 worldPos = basePos + localOffset;
	m_bgObj->transform().position() = worldPos;
	m_fillObj->transform().position() = worldPos;

	// プレイヤー方向に向ける（Y 軸のみ回転）
	// Scene の Health を参照して、最初の Health の位置をプレイヤーと見なす
	auto scene = gameObject().scenePtr();
	if (scene)
	{
		auto healths = scene->GetComponents<Health>();
		if (!healths.empty())
		{
			GameObject* playerObj = &(healths.front()->gameObject());
			if (playerObj)
			{
				Vector3 playerPos = playerObj->transform().position();
				Vector3 dir = playerPos - worldPos;
				// XZ 平面で角度
				float yaw = atan2f(dir.x, dir.z); // ラジアン
				float yawDeg = yaw * (180.0f / 3.14159265358979323846f);

				Quaternion rotQ = m_bgObj->transform().rotation();
				Vector3 rot = { 0.0f, 0.0f, 0.0f };
				rot.y = rotQ.ToEulerAngles().y;
				rot.y = yawDeg;
				m_bgObj->transform().rotation() = Quaternion::FromEulerAngles(rot);
				m_fillObj->transform().rotation() = Quaternion::FromEulerAngles(rot);
			}
		}
	}

	// 進捗取得（0..1）
	float progress = m_powerPlant->GetHoldProgress();
	if (m_powerPlant->IsRestored())
	{
		progress = 1.0f;
	}

	// percent 0..100
	int p = static_cast<int>(std::lround(progress * 100.0f));
	if (p < 0) p = 0;
	if (p > 100) p = 100;

	// メッシュキャッシュを使い、V 範囲を 0..progress にしてメッシュ差し替え
	auto& rendering = gameObject().rendering();
	GraphicsDevice& device = rendering.GetGraphicsDevice();

	if (m_meshCache.find(p) == m_meshCache.end())
	{
		// CreateUIQuadWithVRange は v0..v1 指定（ここでは下=0 上=progress）
		m_meshCache[p] = CreateUIQuadWithVRange(device, 0.0f, progress);
	}

	if (m_meshCache[p])
	{
		m_fillRenderer->mesh = m_meshCache[p].get();
	}

	// 見た目上、オブジェクトのスケールも縦方向に合わせる（フィルがメッシュで上下マスクされるので補助的）
	Vector3 newScale = m_originalScale;
	newScale.y = m_originalScale.y * progress;
	m_fillObj->transform().scale() = newScale;

	// 背景は常に元スケール
	m_bgObj->transform().scale() = m_originalScale;
}