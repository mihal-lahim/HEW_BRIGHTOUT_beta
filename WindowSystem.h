
#ifndef WINDOW_SYSTEM_H
#define WINDOW_SYSTEM_H

#include <functional>
#include <Windows.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Window.h"

class EngineCore;

// ウィンドウメッセージコールバック関数の型定義
using WindowMessageHandler = std::function<void(Window*, UINT, WPARAM, LPARAM)>;

class WindowSystem
{
public:
	WindowSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{ 
		Initialize();
	}
	~WindowSystem() { Finalize(); }

	void Initialize();
	void Finalize();

	// ウィンドウメッセージコールバックの登録
	void RegisterHandler(UINT message, WindowMessageHandler handler) { m_handlers[message].push_back(handler); }

	// ウィンドウメッセージのディスパッチ
	void ProcessMessage(Window* window, UINT msg, WPARAM wp, LPARAM lp);

	// ウィンドウの登録
	void RegisterWindow(Window* window);
private:
	// エンジンコアへのポインタ
	EngineCore* m_engineCore = nullptr;

	// 管理しているウィンドウ
	std::unique_ptr<Window> m_window;

	// ウィンドウメッセージハンドラのマップ
	std::unordered_map<UINT, std::vector<WindowMessageHandler>> m_handlers;
};


#endif
