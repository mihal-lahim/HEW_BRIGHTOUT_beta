#include "TimerUI.h"
#include "GameObject.h"
#include "SceneSystem.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "UIQuad.h"
#include "UIDrawer.h"
#include "Mesh.h"
#include "Result.h"

#include <chrono>
#include <cmath>

using namespace std::chrono;

// π定数
static constexpr float PI = 3.14159265358979323846f;

TimerUI::TimerUI()
{
	// 初期化は Start() へ
}

void TimerUI::Start()
{
	auto* scene = gameObject().scenePtr();
	auto& rendering = gameObject().rendering();
	GraphicsDevice& device = rendering.GetGraphicsDevice();

	// 番号テクスチャは 0..9 を横に並べた1枚画像。U幅は 1/10
	const float digitCount = 10.0f;
	const float uWidth = 1.0f / digitCount;

	// 0..9 の切り出しメッシュを作成（再利用する）
	m_digitMeshes.resize(10);
	for (int i = 0; i < 10; ++i)
	{
		float u0 = uWidth * i;
		float u1 = uWidth * (i + 1);
		m_digitMeshes[i] = CreateUIQuadWithUV(device, u0, u1);
	}

	// 桁数：MM:SS を想定して 4 桁（M10 M1 S10 S1）
	const int digits = 4;
	m_digitRenderers.resize(digits, nullptr);
	m_digitObjects.resize(digits, nullptr);

	// タイマーの基準位置（画面座標, 調整可）
	Vector3 basePos = gameObject().transform().position();

	// 横並びオフセット
	const float digitSpacing = 60.0f;
	const float digitWidth = 50.0f;
	const float digitHeight = 100.0f;

	m_baseDigitWidth = digitWidth;
	m_baseDigitHeight = digitHeight;

	// 左から tens of minutes ... ones of seconds (M10 M1 S10 S1)
	float startX = basePos.x - (digitSpacing * 1.5f);

	for (int i = 0; i < digits; ++i)
	{
		GameObject* digitObj = scene->CreateGameObject();
		digitObj->SetName(std::string("TimerDigit") + std::to_string(i));
		digitObj->transform().position() = Vector3(startX + i * digitSpacing, basePos.y, basePos.z);
		digitObj->transform().scale() = Vector3(digitWidth, digitHeight, 1.0f);

		MeshRenderer* renderer = digitObj->AddComponent<MeshRenderer>();
		renderer->mesh = m_digitMeshes[0].get();
		renderer->renderQueue = RenderQueue::UI;

		renderer->material.vsPath = "UiVS.cso";
		renderer->material.psPath = "UiPS.cso";
		renderer->material.texturePath = L"texture/BRIGHTOUT_Number_0-9.png";

		renderer->material.SetFloat("ScreenWidth", static_cast<float>(device.GetBackBufferWidth()));
		renderer->material.SetFloat("ScreenHeight", static_cast<float>(device.GetBackBufferHeight()));
		renderer->material.SetFloat("UseScreenSpace", 1.0f);

		m_digitRenderers[i] = renderer;
		m_digitObjects[i] = digitObj;
	}

	// TIME UP の UI を事前に作成し、非表示にしておく
	GameObject* timeUpRoot = scene->CreateGameObject();
	timeUpRoot->SetName("TimeUpUIRoot");
	m_timeUpUI = UI::CreateUI(
		timeUpRoot,
		L"texture/time-up.png",
		Vector3(500.0f, 270.0f, 0.0f),
		Vector3(960.0f, 540.0f, 1.0f),
		"UiVS.cso",
		"UiPS.cso"
	);
	// UIオブジェクトと親ルートの両方を非表示にする
	if (m_timeUpUI)
	{
		m_timeUpUI->SetActive(false);
	}
	timeUpRoot->SetActive(false);
	m_timeUpRoot = timeUpRoot;

	m_lastTime = steady_clock::now();
}

void TimerUI::Update()
{
	// --- TIME UP 表示中：遷移ディレイ ---
	if (m_isTimeUp)
	{
		auto now = steady_clock::now();
		duration<float> delta = now - m_lastTime;
		m_lastTime = now;

		m_timeUpElapsed += delta.count();
		if (m_timeUpElapsed >= m_timeUpDelay && !m_hasTransitioned)
		{
			m_hasTransitioned = true;
			scene().ChangeScene<Result>();
		}
		return;
	}

	// 経過時間を計算して残り時間をデクリメント
	auto now = steady_clock::now();
	duration<float> delta = now - m_lastTime;
	m_lastTime = now;

	m_totalSeconds -= delta.count();
	if (m_totalSeconds < 0.0f) m_totalSeconds = 0.0f;

	// --- タイマーが0になったら TIME UP 演出 ---
	if (m_totalSeconds <= 0.0f && !m_isTimeUp)
	{
		m_isTimeUp = true;
		m_timeUpElapsed = 0.0f;

		// タイマー数字を非表示にする
		for (auto* obj : m_digitObjects)
		{
			if (obj) obj->SetActive(false);
		}

		// TIME UP の UI を表示
		if (m_timeUpRoot)
		{
			m_timeUpRoot->SetActive(true);
		}
		if (m_timeUpUI)
		{
			m_timeUpUI->SetActive(true);
		}

		return;
	}

	// --- 現在の各桁を計算 ---
	int totalSec = static_cast<int>(std::floor(m_totalSeconds + 0.0001f));

	int minutes = totalSec / 60;
	int seconds = totalSec % 60;

	int m10 = (minutes / 10) % 10;
	int m1 = minutes % 10;
	int s10 = (seconds / 10) % 10;
	int s1 = seconds % 10;

	int digits[4] = { m10, m1, s10, s1 };

	// --- 残り5秒以下：秒の1の位が変わったら拡大開始 ---
	if (m_totalSeconds <= 6.0f && m_totalSeconds > 0.0f)
	{
		// 秒の1の位が変わったか検出
		if (m_prevOnesDigit != s1)
		{
			m_prevOnesDigit = s1;
			m_digitChangeElapsed = 0.0f;
			m_isPulsing = true;
		}

		// 拡大縮小演出中
		if (m_isPulsing)
		{
			m_digitChangeElapsed += delta.count();

			// 1秒かけて最大倍率 → 基準倍率に縮小（線形補間）
			float t = m_digitChangeElapsed; // 0.0 ~ 1.0
			if (t >= 1.0f)
			{
				t = 1.0f;
				m_isPulsing = false;
			}

			// t=0 で最大、t=1 で基準サイズ
			float scale = m_pulseMaxScale + (1.0f - m_pulseMaxScale) * t;

			float scaledW = m_baseDigitWidth * scale;
			float scaledH = m_baseDigitHeight * scale;

			// 最後の桁（s1 = index 3）だけスケール変更
			if (m_digitObjects[3])
			{
				m_digitObjects[3]->transform().scale() = Vector3(scaledW, scaledH, 1.0f);
			}
		}
	}
	else
	{
		// 5秒より前は初期化しておく
		m_prevOnesDigit = s1;
		m_isPulsing = false;

		// 全桁を基準スケールに保つ
		if (m_digitObjects[3])
		{
			m_digitObjects[3]->transform().scale() = Vector3(m_baseDigitWidth, m_baseDigitHeight, 1.0f);
		}
	}

	// 各レンダラにメッシュを差し替え
	for (int i = 0; i < 4; ++i)
	{
		if (m_digitRenderers[i] && m_digitMeshes.size() > static_cast<size_t>(digits[i]))
		{
			m_digitRenderers[i]->mesh = m_digitMeshes[digits[i]].get();
			m_digitRenderers[i]->material.texturePath = L"texture/BRIGHTOUT_Number_0-9.png";
			m_digitRenderers[i]->material.vsPath = "UiVS.cso";
			m_digitRenderers[i]->material.psPath = "UiPS.cso";
		}
	}
}