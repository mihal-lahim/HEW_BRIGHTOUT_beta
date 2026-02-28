#include "MorphUI.h"
#include "GameObject.h"
#include "SceneSystem.h"
#include "PlayerMorphsystem.h"

using namespace std;

void MorphUI::Start()
{
	// タイマー初期化
	m_lastTime = chrono::steady_clock::now();
	m_timer = 0.0f;
	m_state = State::Available;

	// Scene から PlayerMorphSystem を探す（最初のものを使用）
	auto* scene = gameObject().scenePtr();
	if (scene)
	{
		auto comps = scene->GetComponents<PlayerMorphSystem>();
		if (!comps.empty())
		{
			m_morphSystem = comps.front();
		}
	}

	// 初期 canMorph を取得（未取得なら true として trans02 を表示）
	bool canMorph = (m_morphSystem) ? m_morphSystem->CanMorph() : true;
	m_prevCanMorph = canMorph;

	// UI の初期表示は通常 trans02（変身可能）を表示、trans01 を隠す
	if (uiAvailable) uiAvailable->SetActive(true);
	if (uiActive) uiActive->SetActive(false);
}

void MorphUI::Update()
{
	// 時間差取得
	auto now = chrono::steady_clock::now();
	float delta = chrono::duration<float>(now - m_lastTime).count();
	m_lastTime = now;

	// morphSystem が無ければ毎フレーム再探索
	if (!m_morphSystem)
	{
		auto* scene = gameObject().scenePtr();
		if (scene)
		{
			auto comps = scene->GetComponents<PlayerMorphSystem>();
			if (!comps.empty())
			{
				m_morphSystem = comps.front();
			}
		}
	}

	bool canMorph = (m_morphSystem) ? m_morphSystem->CanMorph() : true;

	// 状態遷移検出
	if (m_state == State::Available)
	{
		// 以前は変身可能で、現在は変身不可になったら「変身した」とみなす
		if (m_prevCanMorph && !canMorph)
		{
			// 変身開始：trans02 を隠して trans01 を表示、クールダウン開始
			if (uiAvailable) uiAvailable->SetActive(false);
			if (uiActive) uiActive->SetActive(true);
			m_timer = 0.0f;
			m_state = State::ActiveCooldown;
		}
		else
		{
			// 通常は trans02 を canMorph に合わせて表示（範囲外などで非表示）
			if (uiAvailable) uiAvailable->SetActive(canMorph);
			if (uiActive) uiActive->SetActive(!canMorph);
		}
	}
	else // ActiveCooldown
	{
		m_timer += delta;
		if (m_timer >= cooldownSeconds)
		{
			// クールダウン終了 → trans02 を表示に戻す
			if (uiActive) uiActive->SetActive(false);
			if (uiAvailable) uiAvailable->SetActive(true);
			m_state = State::Available;
			// タイマークリア
			m_timer = 0.0f;
		}
	}

	m_prevCanMorph = canMorph;
}