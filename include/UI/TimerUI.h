#pragma once

#include "ScriptComponent.h"
#include <vector>
#include <memory>
#include <chrono>

class Mesh;
class MeshRenderer;
class GameObject;
class FadeController;

class TimerUI : public ScriptComponent
{
public:
    TimerUI();

    void Start() override;
    void Update() override;

	// 残り秒数を外部から取得するためのゲッター
	float GetRemainingSeconds() const { return m_totalSeconds; }

	// 残り時間を指定秒数だけ減らす（0未満にはならない）
	void SubtractSeconds(float seconds) { m_totalSeconds -= seconds; if (m_totalSeconds < 0.0f) m_totalSeconds = 0.0f; }

private:
    // カウントダウン秒数
    float m_totalSeconds = 210.0f;

    // シーン遷移済みフラグ（二重遷移防止）
    bool m_hasTransitioned = false;

    // 0..9 の切り出しメッシュ（shared_ptr で保持しておく）
    std::vector<std::shared_ptr<Mesh>> m_digitMeshes;

    // 各桁の MeshRenderer を保持
    std::vector<MeshRenderer*> m_digitRenderers;

    // 各桁の GameObject を保持（スケール変更用）
    std::vector<GameObject*> m_digitObjects;

    // 差分時間計測
    std::chrono::steady_clock::time_point m_lastTime;

    // --- 残り5秒演出用 ---
    float m_baseDigitWidth = 50.0f;
    float m_baseDigitHeight = 100.0f;
    float m_pulseMaxScale = 4.0f;
    int m_prevOnesDigit = -1;
    float m_digitChangeElapsed = 0.0f;
    bool m_isPulsing = false;

    // --- TIME UP 演出用 ---
    GameObject* m_timeUpUI = nullptr;
    GameObject* m_timeUpRoot = nullptr;
    float m_timeUpDelay = 2.0f;
    float m_timeUpElapsed = 0.0f;
    bool m_isTimeUp = false;

    // --- フェードアウト用 ---
    FadeController* m_fadeController = nullptr;

    // --- ブザーSE ---
    int m_buzzerSE = -1;
};