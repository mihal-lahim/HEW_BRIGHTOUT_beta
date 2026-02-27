#pragma once

#include "ScriptComponent.h"
#include <vector>
#include <memory>
#include <chrono>

class Mesh;
class MeshRenderer;
class GameObject;

class TimerUI : public ScriptComponent
{
public:
    TimerUI();

    void Start() override;
    void Update() override;

	// 残り秒数を外部から参照するためのゲッター
	float GetRemainingSeconds() const { return m_totalSeconds; }

private:
    // カウントダウン秒数
    float m_totalSeconds = 120.0f;

    // シーン遷移済みフラグ（二重遷移防止）
    bool m_hasTransitioned = false;

    // 0..9 の切り出しメッシュ（shared_ptr で保持しておく）
    std::vector<std::shared_ptr<Mesh>> m_digitMeshes;

    // 各桁の MeshRenderer を保持
    std::vector<MeshRenderer*> m_digitRenderers;

    // 各桁の GameObject を保持（スケール操作用）
    std::vector<GameObject*> m_digitObjects;

    // 内部時間計測
    std::chrono::steady_clock::time_point m_lastTime;

    // --- 残り5秒演出用 ---
    // 拡大縮小の基準スケール
    float m_baseDigitWidth = 50.0f;
    float m_baseDigitHeight = 100.0f;

    // 拡大の最大倍率
    float m_pulseMaxScale = 4.0f;

    // 前フレームの秒の1の位（変化検出用）
    int m_prevOnesDigit = -1;

    // 秒が変わってからの経過時間（0.0 ~ 1.0秒）
    float m_digitChangeElapsed = 0.0f;

    // 拡大縮小演出中かどうか
 bool m_isPulsing = false;

    // --- TIME UP 演出用 ---
    // TIME UP の UI オブジェクト（初期非表示、UI::CreateUIが返すオブジェクト）
    GameObject* m_timeUpUI = nullptr;

    // TIME UP の UI の親ルート
    GameObject* m_timeUpRoot = nullptr;

    // TIME UP 表示後の遷移までの待機秒数
    float m_timeUpDelay = 2.0f;

    // TIME UP 表示後の経過時間
    float m_timeUpElapsed = 0.0f;

    // TIME UP 状態かどうか
    bool m_isTimeUp = false;
};