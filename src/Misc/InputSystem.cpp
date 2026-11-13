#include "InputSystem.h"
#include "EngineCore.h"
#include "GameContext.h"
#include "WindowSystem.h"

void InputSystem::Initialize()
{
	if (!m_engineCore)
	{
		return;
	}

	GameContext& context = m_engineCore->GetGameContext();
	if (!context.windowSystem)
	{
		return;
	}

	auto keyboardHandler = [this](Window*, UINT msg, WPARAM wp, LPARAM lp)
	{
		m_keyboard.ProcessMessage(msg, wp, lp);
	};

	context.windowSystem->RegisterHandler(WM_ACTIVATEAPP, keyboardHandler);
	context.windowSystem->RegisterHandler(WM_KEYDOWN, keyboardHandler);
	context.windowSystem->RegisterHandler(WM_SYSKEYDOWN, keyboardHandler);
	context.windowSystem->RegisterHandler(WM_KEYUP, keyboardHandler);
	context.windowSystem->RegisterHandler(WM_SYSKEYUP, keyboardHandler);
}

void InputSystem::Update()
{
	m_keyboard.InputUpdate();
	for (auto& gamePad : m_gamePads)
	{
		gamePad.InputUpdate();
	}
}

void InputSystem::Finalize()
{
	// “Á‚ÉI—¹ˆ—‚Í•s—v
}
