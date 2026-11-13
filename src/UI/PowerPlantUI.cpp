#include "PowerPlantUI.h"
#include "SceneSystem.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "BillboardQuad.h"
#include "Mesh.h"
#include "PowerPlant.h"
#include <cmath>

using namespace std;

// 下揃えビルボード用クワッド（Y: 0?1, X: -0.5?0.5）
static std::shared_ptr<Mesh> CreateBottomAlignedBillboardQuad(GraphicsDevice& device)
{
	using VA = Mesh::VertexAttribute;
	std::vector<VA> vertices(4);

	// 左上
	vertices[0].position = { -0.5f, 1.0f, 0.0f };
	vertices[0].color    = {  1.0f, 1.0f, 1.0f, 1.0f };
	vertices[0].normal   = {  0.0f, 0.0f, -1.0f };
	vertices[0].uv       = {  0.0f, 0.0f };

	// 右上
	vertices[1].position = {  0.5f, 1.0f, 0.0f };
	vertices[1].color    = {  1.0f, 1.0f, 1.0f, 1.0f };
	vertices[1].normal   = {  0.0f, 0.0f, -1.0f };
	vertices[1].uv       = {  1.0f, 0.0f };

	// 左下
	vertices[2].position = { -0.5f, 0.0f, 0.0f };
	vertices[2].color    = {  1.0f, 1.0f, 1.0f, 1.0f };
	vertices[2].normal   = {  0.0f, 0.0f, -1.0f };
	vertices[2].uv       = {  0.0f, 1.0f };

	// 右下
	vertices[3].position = {  0.5f, 0.0f, 0.0f };
	vertices[3].color    = {  1.0f, 1.0f, 1.0f, 1.0f };
	vertices[3].normal   = {  0.0f, 0.0f, -1.0f };
	vertices[3].uv       = {  1.0f, 1.0f };

	// 両面描画: 表面 + 裏面のインデックスを追加
	// ビルボードはカメラ角度によってワインディングが反転するため両面必要
	std::vector<UINT> indices = {
		0, 1, 2, 1, 3, 2,  // 表面
		0, 2, 1, 1, 2, 3   // 裏面
	};

	auto mesh = std::make_shared<Mesh>();
	mesh->CreateBuffer(device, vertices, indices);
	return mesh;
}

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

	// 下揃えビルボード用メッシュ作成
	m_billboardQuad = CreateBottomAlignedBillboardQuad(device);

	// 背景オブジェクト
	m_bgObj = scene->CreateGameObject();
	m_bgObj->SetName("PowerPlantUI_BG");
	Vector3 basePos = gameObject().transform().position();
	m_bgObj->transform().position() = basePos + localOffset;
	m_bgObj->transform().scale() = uiScale;

	m_bgRenderer = m_bgObj->AddComponent<MeshRenderer>();
	m_bgRenderer->mesh = m_billboardQuad.get();
	m_bgRenderer->renderQueue = RenderQueue::Transparent;
	m_bgRenderer->material.texturePath = backgroundTexture;
	m_bgRenderer->material.vsPath = "BillboardVS.cso";
	m_bgRenderer->material.psPath = "BillboardPS.cso";
	m_bgRenderer->material.shaderProgram = nullptr;
	m_bgRenderer->material.SetFloat4("uv_rect", { 0.0f, 0.0f, 1.0f, 1.0f });

	// フィルオブジェクト（背景の前面に重ねる）
	m_fillObj = scene->CreateGameObject();
	m_fillObj->SetName("PowerPlantUI_Fill");
	m_fillObj->transform().position() = basePos + localOffset;
	m_fillObj->transform().scale() = Vector3(uiScale.x, 0.0f, uiScale.z);

	m_fillRenderer = m_fillObj->AddComponent<MeshRenderer>();
	m_fillRenderer->mesh = m_billboardQuad.get();
	m_fillRenderer->renderQueue = RenderQueue::Transparent;
	m_fillRenderer->material.texturePath = fillTexture;
	m_fillRenderer->material.vsPath = "BillboardVS.cso";
	m_fillRenderer->material.psPath = "BillboardPS.cso";
	m_fillRenderer->material.shaderProgram = nullptr;
	m_fillRenderer->material.SetFloat4("uv_rect", { 0.0f, 1.0f, 1.0f, 0.0f });
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

	// フィルを背景より少し上にオフセットしてZファイティング防止
	Vector3 fillPos = worldPos;
	fillPos.y += 0.01f;
	m_fillObj->transform().position() = fillPos;

	// 進捗取得（0..1）
	float progress = m_powerPlant->GetHoldProgress();
	if (m_powerPlant->IsRestored())
	{
		progress = 1.0f;
	}

	// フィルのスケールとUVを進捗に合わせる
	Vector3 newScale = m_originalScale;
	newScale.y = m_originalScale.y * progress;
	m_fillObj->transform().scale() = newScale;

	// uv_rect で表示するテクスチャ範囲を制御（下部分から伸びる）
	float vOffset = 1.0f - progress;
	float vScale = progress;
	m_fillRenderer->material.SetFloat4("uv_rect", { 0.0f, vOffset, 1.0f, vScale });

	// 背景は常に元スケール
	m_bgObj->transform().scale() = m_originalScale;
}