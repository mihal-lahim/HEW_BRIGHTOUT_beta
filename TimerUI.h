#pragma once

#include "ScriptComponent.h"
#include <vector>
#include <memory>
#include <chrono>

class Mesh;
class MeshRenderer;

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
    float m_totalSeconds = 240.0f;

    // 0..9 の切り出しメッシュ（shared_ptr で保持しておく）
    std::vector<std::shared_ptr<Mesh>> m_digitMeshes;

    // 各桁の MeshRenderer を保持
    std::vector<MeshRenderer*> m_digitRenderers;

    // 内部時間計測
    std::chrono::steady_clock::time_point m_lastTime;
};