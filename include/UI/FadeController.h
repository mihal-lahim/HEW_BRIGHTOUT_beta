#ifndef FADE_CONTROLLER_H
#define FADE_CONTROLLER_H

#include "ScriptComponent.h"
#include <functional>
#include <chrono>

class MeshRenderer;

// フェードの種類
enum class FadeType
{
	None,
	FadeIn,   // 黒→明るい（画面が見えてくる）
	FadeOut,  // 明るい→黒（画面が暗くなる）
};

class FadeController : public ScriptComponent
{
public:
	// フェード時間（秒）
	float fadeDuration = 1.0f;

	// フェード完了時のコールバック
	std::function<void()> onFadeComplete = nullptr;

	void Start() override;
	void Update() override;

	// フェードイン開始（黒→明るい）
	void StartFadeIn(float duration = 1.0f, std::function<void()> callback = nullptr);

	// フェードアウト開始（明るい→黒）
	void StartFadeOut(float duration = 1.0f, std::function<void()> callback = nullptr);

	// フェード中かどうか
	bool IsFading() const { return m_fadeType != FadeType::None; }

private:
	FadeType m_fadeType = FadeType::None;
	float m_fadeTimer = 0.0f;
	float m_currentAlpha = 0.0f;

	MeshRenderer* m_renderer = nullptr;
	GameObject* m_fadeQuad = nullptr;

	// Start()前にフェード開始が呼ばれた場合の予約フラグ
	bool m_pendingFade = false;

	// Update()初回フレーム用フラグ（デルタタイム暴走防止）
	bool m_firstFrame = true;

	std::chrono::steady_clock::time_point m_lastTime{};

	void UpdateAlpha(float alpha);
};

#endif