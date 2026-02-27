#include "FadeController.h"
#include "GameObject.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "UIDrawer.h"
#include <algorithm>
#include <chrono>
#include <Windows.h>

using namespace std::chrono;

void FadeController::Start()
{
	m_lastTime = steady_clock::now();

	OutputDebugStringA("[FadeController] Start() called\n");

	// フェード用の全画面UIを作成（白画像をシェーダーで黒く塗る）
	m_fadeQuad = UI::CreateUI(
		&gameObject(),
		L"texture/fade.png",
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(1920.0f, 1080.0f, 1.0f),
		"UiVS.cso",
		"FadePS.cso"
	);

	if (m_fadeQuad)
	{
		m_renderer = m_fadeQuad->GetComponent<MeshRenderer>();
		if (m_renderer)
		{
			m_renderer->renderQueue = RenderQueue::UI;
			m_renderer->material.psPath = "FadePS.cso";
		}
	}

	// Start前にStartFadeIn/Outが予約されていた場合、ここで適用
	if (m_pendingFade)
	{
		m_pendingFade = false;
		m_firstFrame = true;  // Update初回でm_lastTimeをリセットさせる
		UpdateAlpha(m_currentAlpha);
		if (m_fadeQuad) m_fadeQuad->SetActive(true);

		OutputDebugStringA("[FadeController] Pending fade applied\n");
	}
	else
	{
		// 初期状態は完全透明（非表示）
		m_currentAlpha = 0.0f;
		UpdateAlpha(m_currentAlpha);
		if (m_fadeQuad) m_fadeQuad->SetActive(false);
	}
}

void FadeController::Update()
{
	if (m_fadeType == FadeType::None) return;

	auto now = steady_clock::now();

	// 初回フレーム: デルタタイムの暴走を防ぐためタイムスタンプをリセット
	if (m_firstFrame)
	{
		m_firstFrame = false;
		m_lastTime = now;
		// 初回は alpha を適用するだけで進めない
		UpdateAlpha(m_currentAlpha);
		return;
	}

	duration<float> delta = now - m_lastTime;
	m_lastTime = now;

	m_fadeTimer += delta.count();

	float t = std::clamp(m_fadeTimer / fadeDuration, 0.0f, 1.0f);

	switch (m_fadeType)
	{
	case FadeType::FadeIn:
		// 画面が黒い状態(alpha=1.0) → 段々明るくなる(alpha=0.0)
		m_currentAlpha = 1.0f - t;
		break;
	case FadeType::FadeOut:
		// 画面が明るい状態(alpha=0.0) → 段々暗くなる(alpha=1.0)
		m_currentAlpha = t;
		break;
	default:
		break;
	}

	UpdateAlpha(m_currentAlpha);

	// フェード完了
	if (t >= 1.0f)
	{
		FadeType completedType = m_fadeType;
		m_fadeType = FadeType::None;

		OutputDebugStringA("[FadeController] Fade complete\n");

		// フェードインが完了したらオーバーレイを非表示にする
		if (completedType == FadeType::FadeIn && m_fadeQuad)
		{
			m_fadeQuad->SetActive(false);
		}

		if (onFadeComplete)
		{
			auto callback = onFadeComplete;
			onFadeComplete = nullptr;
			callback();
		}
	}
}

void FadeController::StartFadeIn(float duration, std::function<void()> callback)
{
	fadeDuration = duration;
	onFadeComplete = callback;
	m_fadeType = FadeType::FadeIn;
	m_fadeTimer = 0.0f;
	m_currentAlpha = 1.0f;
	m_firstFrame = true;

	OutputDebugStringA("[FadeController] StartFadeIn called\n");

	// Startがまだ呼ばれていない場合は予約だけしておく
	if (!m_renderer)
	{
		m_pendingFade = true;
		OutputDebugStringA("[FadeController] Pending (renderer not ready)\n");
		return;
	}

	UpdateAlpha(m_currentAlpha);
	if (m_fadeQuad) m_fadeQuad->SetActive(true);
}

void FadeController::StartFadeOut(float duration, std::function<void()> callback)
{
	fadeDuration = duration;
	onFadeComplete = callback;
	m_fadeType = FadeType::FadeOut;
	m_fadeTimer = 0.0f;
	m_currentAlpha = 0.0f;
	m_firstFrame = true;

	OutputDebugStringA("[FadeController] StartFadeOut called\n");

	// Startがまだ呼ばれていない場合は予約だけしておく
	if (!m_renderer)
	{
		m_pendingFade = true;
		OutputDebugStringA("[FadeController] Pending (renderer not ready)\n");
		return;
	}

	UpdateAlpha(m_currentAlpha);
	if (m_fadeQuad) m_fadeQuad->SetActive(true);
}

void FadeController::UpdateAlpha(float alpha)
{
	if (!m_renderer) return;

	// 白テクスチャ × (R=0, G=0, B=0, A=alpha) = 黒の半透明オーバーレイ
	m_renderer->material.SetColor({ 0.0f, 0.0f, 0.0f, alpha });
}