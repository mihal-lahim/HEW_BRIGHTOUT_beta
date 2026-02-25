#pragma once
#include "ScriptComponent.h"
#include <chrono>

class GameObject;
class PlayerMorphSystem;

class MorphUI : public ScriptComponent
{
public:
	GameObject* uiAvailable = nullptr; // trans02.png (変身可能)
	GameObject* uiActive = nullptr;    // trans01.png (変身中 / クールダウン中)
	float cooldownSeconds = 3.0f;    //クールタイム

	void Start() override;
	void Update() override;

private:
	PlayerMorphSystem* m_morphSystem = nullptr;
	bool m_prevCanMorph = false;
	enum class State { Available, ActiveCooldown } m_state = State::Available;
	std::chrono::steady_clock::time_point m_lastTime;
	float m_timer = 0.0f;
};