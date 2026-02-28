#pragma once

#include "ScriptComponent.h"

class TimerUI;

class BgmSwitcher : public ScriptComponent
{
public:
	// Namioka などから設定する
	int currentIndex = -1; // 現在再生中のBGMインデックス
	int nextIndex = -1;    // 切り替える先のBGMインデックス

	// 残り何秒で切り替えるか（デフォルト 60 秒）
	float switchAtSeconds = 60.0f;

	void Start() override;
	void Update() override;

private:
	TimerUI* m_timerUI = nullptr;
	bool m_switched = false;
};