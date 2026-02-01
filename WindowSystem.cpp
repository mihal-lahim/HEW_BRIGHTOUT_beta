#include "WindowSystem.h"
#include "Window.h"

void WindowSystem::Initialize()
{
	m_handlers.clear();
	m_window.reset();
}

void WindowSystem::Finalize()
{
	m_handlers.clear();
	m_window.reset();
}

void WindowSystem::ProcessMessage(Window* window, UINT msg, WPARAM wp, LPARAM lp)
{
	auto it = m_handlers.find(msg);
	if (it != m_handlers.end())
	{
		// “o˜^‚³‚ê‚Ä‚¢‚éƒnƒ“ƒhƒ‰‚ð‚·‚×‚ÄŒÄ‚Ño‚·
		for (const auto& handler : it->second)
		{
			handler(window, msg, wp, lp);
		}
	}
}

void WindowSystem::RegisterWindow(Window* window)
{
	m_window.reset(window);
	m_window->m_windowSystem = this;
}
