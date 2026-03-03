#include "PowerPlantCountUI.h"
#include "GameObject.h"
#include "SceneSystem.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "UIQuad.h"
#include "UIDrawer.h"
#include "Mesh.h"
#include "ScoreData.h"

void PowerPlantCountUI::Start()
{
	auto* scene = gameObject().scenePtr();
	auto& rendering = gameObject().rendering();
	GraphicsDevice& device = rendering.GetGraphicsDevice();

	// 番号テクスチャは 0..9 を横に並べた1枚画像。U幅は 1/10
	const float digitCount = 10.0f;
	const float uWidth = 1.0f / digitCount;

	// 0..9 の切り出しメッシュを作成
	m_digitMeshes.resize(10);
	for (int i = 0; i < 10; ++i)
	{
		float u0 = uWidth * i;
		float u1 = uWidth * (i + 1);
		m_digitMeshes[i] = CreateUIQuadWithUV(device, u0, u1);
	}

	// --- アイコン表示 ---
	{
		m_iconObject = UI::CreateUI(
			&gameObject(),
			iconTexturePath,
			iconPosition,
			iconScale,
			"UiVS.cso",
			"UiPS.cso"
		);
		if (m_iconObject)
		{
			m_iconObject->SetName("PlantCountIcon");
		}
	}

	// --- 現在数（分子）の数字 ---
	{
		GameObject* digitObj = scene->CreateGameObject();
		digitObj->SetName("PlantCountCurrent");
		digitObj->transform().position() = currentDigitPosition;
		digitObj->transform().scale() = currentDigitScale;

		MeshRenderer* renderer = digitObj->AddComponent<MeshRenderer>();
		renderer->mesh = m_digitMeshes[0].get();
		renderer->renderQueue = RenderQueue::UI;

		renderer->material.vsPath = "UiVS.cso";
		renderer->material.psPath = "UiPS.cso";
		renderer->material.texturePath = L"texture/Number_ver2.png";

		renderer->material.SetFloat("ScreenWidth", static_cast<float>(device.GetBackBufferWidth()));
		renderer->material.SetFloat("ScreenHeight", static_cast<float>(device.GetBackBufferHeight()));
		renderer->material.SetFloat("UseScreenSpace", 1.0f);

		m_currentDigitRenderer = renderer;
		m_currentDigitObject = digitObj;
	}

	// --- 最大数（分母）の数字 ---
	{
		int totalDigit = totalPlantCount % 10; // 1桁想定

		GameObject* digitObj = scene->CreateGameObject();
		digitObj->SetName("PlantCountTotal");
		digitObj->transform().position() = totalDigitPosition;
		digitObj->transform().scale() = totalDigitScale;

		MeshRenderer* renderer = digitObj->AddComponent<MeshRenderer>();
		renderer->mesh = m_digitMeshes[totalDigit].get();
		renderer->renderQueue = RenderQueue::UI;

		renderer->material.vsPath = "UiVS.cso";
		renderer->material.psPath = "UiPS.cso";
		renderer->material.texturePath = L"texture/Number_ver2.png";

		renderer->material.SetFloat("ScreenWidth", static_cast<float>(device.GetBackBufferWidth()));
		renderer->material.SetFloat("ScreenHeight", static_cast<float>(device.GetBackBufferHeight()));
		renderer->material.SetFloat("UseScreenSpace", 1.0f);

		m_totalDigitRenderer = renderer;
		m_totalDigitObject = digitObj;
	}
}

void PowerPlantCountUI::Update()
{
	// ScoreData から復旧済み数を取得
	int currentCount = ScoreData::Instance().restoredPlants;

	// 値が変わった場合のみメッシュを差し替え
	if (currentCount != m_prevCurrentCount)
	{
		m_prevCurrentCount = currentCount;

		int digit = currentCount % 10; // 1桁想定（0?9）
		if (m_currentDigitRenderer && digit >= 0 && digit < static_cast<int>(m_digitMeshes.size()))
		{
			m_currentDigitRenderer->mesh = m_digitMeshes[digit].get();
		}
	}
}
