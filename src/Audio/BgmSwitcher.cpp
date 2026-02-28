#include "BgmSwitcher.h"
#include "TimerUI.h"
#include "SceneSystem.h"
#include "GameObject.h"
#include "audio.h"

void BgmSwitcher::Start()
{
	m_switched = false;

	// Scene から TimerUI を探す（最初のものを利用）
	auto* scene = gameObject().scenePtr();
	if (scene)
	{
		auto comps = scene->GetComponents<TimerUI>();
		if (!comps.empty())
		{
			m_timerUI = comps.front();
		}
	}
}

void BgmSwitcher::Update()
{
	if (m_switched)
		return;

	// 必要なインデックスが設定されていない場合は何もしない
	if (currentIndex < 0 || nextIndex < 0)
		return;

	// TimerUI がなければ再探索
	if (!m_timerUI)
	{
		auto* scene = gameObject().scenePtr();
		if (scene)
		{
			auto comps = scene->GetComponents<TimerUI>();
			if (!comps.empty())
			{
				m_timerUI = comps.front();
			}
		}
		if (!m_timerUI)
			return;
	}

	float remaining = m_timerUI->GetRemainingSeconds();

	if (remaining <= switchAtSeconds)
	{
		// 現在のBGMを止めて次をループ再生
		StopAudio(currentIndex);
		PlayAudio(nextIndex, true);
		m_switched = true;
	}
}