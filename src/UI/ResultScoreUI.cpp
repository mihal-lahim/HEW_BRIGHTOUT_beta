#include "ResultScoreUI.h"
#include "ScoreData.h"
#include "GameObject.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "UIQuad.h"
#include "Mesh.h"

void ResultScoreUI::Start()
{
	auto& rendering = gameObject().rendering();
	GraphicsDevice& device = rendering.GetGraphicsDevice();

	// 0~9 の切り出しメッシュを作成
	const float digitCount = 10.0f;
	const float uWidth = 1.0f / digitCount;
	m_digitMeshes.resize(10);
	for (int i = 0; i < 10; ++i)
	{
		float u0 = uWidth * i;
		float u1 = uWidth * (i + 1);
		m_digitMeshes[i] = CreateUIQuadWithUV(device, u0, u1);
	}

	// スコアデータを取得
	const auto& score = ScoreData::Instance();

	// 5項目を描画
	CreateNumberRow(configRestoredCount, score.restoredPlants);
	CreateNumberRow(configRestoredScore, score.GetPlantScore());
	CreateNumberRow(configKilledCount, score.killedEnemies);
	CreateNumberRow(configKilledScore, score.GetEnemyScore());
	CreateNumberRow(configTotalScore, score.GetTotalScore());
}

void ResultScoreUI::CreateNumberRow(const ScoreRowConfig& config, int value)
{
	auto* scene = gameObject().scenePtr();
	auto& rendering = gameObject().rendering();
	GraphicsDevice& device = rendering.GetGraphicsDevice();

	if (value < 0) value = 0;

	// 各桁の数字を抽出（上位桁から）
	std::vector<int> digits;
	if (value == 0)
	{
		digits.push_back(0);
	}
	else
	{
		int temp = value;
		while (temp > 0)
		{
			digits.insert(digits.begin(), temp % 10);
			temp /= 10;
		}
	}

	// maxDigits に足りなければ先頭を 0 で埋める
	while (static_cast<int>(digits.size()) < config.maxDigits)
	{
		digits.insert(digits.begin(), 0);
	}

	// 各桁の GameObject を作成して描画
	for (int i = 0; i < static_cast<int>(digits.size()); ++i)
	{
		GameObject* digitObj = scene->CreateGameObject();
		digitObj->SetName(std::string("ScoreDigit_") + std::to_string(i));
		digitObj->transform().position() = Vector3(
			config.position.x + i * config.digitSpacing,
			config.position.y,
			config.position.z
		);
		digitObj->transform().scale() = config.digitScale;

		MeshRenderer* renderer = digitObj->AddComponent<MeshRenderer>();
		int d = digits[i];
		if (d < 0 || d > 9) d = 0;
		renderer->mesh = m_digitMeshes[d].get();
		renderer->renderQueue = RenderQueue::UI;

		renderer->material.vsPath = "UiVS.cso";
		renderer->material.psPath = "UiPS.cso";
		renderer->material.texturePath = L"texture/BRIGHTOUT_Number_0-9.png";

		renderer->material.SetFloat("ScreenWidth", static_cast<float>(device.GetBackBufferWidth()));
		renderer->material.SetFloat("ScreenHeight", static_cast<float>(device.GetBackBufferHeight()));
		renderer->material.SetFloat("UseScreenSpace", 1.0f);
	}
}
