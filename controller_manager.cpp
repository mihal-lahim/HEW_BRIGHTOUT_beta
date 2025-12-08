#include "controller_manager.h"
#include "controller.h"
#include "debug_console.h"
#include <Xinput.h>

// シングルトンインスタンス
ControllerManager& ControllerManager::GetInstance()
{
	static ControllerManager instance;
	return instance;
}

ControllerManager::ControllerManager()
	: m_initialized(false)
{
	// コントローラー配列を初期化
	for (int i = 0; i < 3; ++i) {
		m_controllers[i] = nullptr;
	}
}

ControllerManager::~ControllerManager()
{
	if (m_initialized) {
		Finalize();
	}
}

void ControllerManager::Initialize()
{
	if (m_initialized) {
		hal::DebugConsole::GetInstance().LogFormat("[ControllerManager] Already initialized");
		return;
	}

	// コントローラーを動的確保し、各スロットごとにコンストラクタで初期化
	for (int i = 0; i < 3; ++i) {
		if (!m_controllers[i]) {
			m_controllers[i] = new Controller(i, 16);
		}
	}

	// 各コントローラーのコールバック設定
	for (int i = 0; i < 3; ++i) {
		if (m_controllers[i]) {
			m_controllers[i]->SetOnConnected([i](int pad) {
				hal::DebugConsole::GetInstance().LogFormat("[Controller] Connected: Pad %d (Slot %d)", pad, i);
			});
			m_controllers[i]->SetOnDisconnected([i](int pad) {
				hal::DebugConsole::GetInstance().LogFormat("[Controller] Disconnected: Pad %d (Slot %d)", pad, i);
			});
			m_controllers[i]->SetOnButtonPress([i](int pad, Controller::Button btn) {
				hal::DebugConsole::GetInstance().LogFormat("[Controller] Button Press: Pad %d Button %d (Slot %d)", pad, static_cast<int>(btn), i);
			});
		}
	}

	m_initialized = true;
	hal::DebugConsole::GetInstance().Success("[ControllerManager] Controllers initialized");
}

void ControllerManager::Finalize()
{
	if (!m_initialized) {
		return;
	}

	// コントローラーを解放（スレッド停止・Join を行う）
	for (int i = 0; i < 3; ++i) {
		if (m_controllers[i]) {
			// 実行中のスレッドがあれば停止してから join
			m_controllers[i]->Stop();
			m_controllers[i]->Join();
			delete m_controllers[i];
			m_controllers[i] = nullptr;
		}
	}

	m_initialized = false;
	hal::DebugConsole::GetInstance().Success("[ControllerManager] Controllers finalized");
}

void ControllerManager::Update()
{
	if (!m_initialized) {
		return;
	}

	// コントローラーを更新
	for (int i = 0; i < 3; ++i) {
		if (m_controllers[i]) {
			m_controllers[i]->Update();
		}
	}
}

Controller* ControllerManager::GetController(int index) const
{
	if (index < 0 || index >= 3) {
		return nullptr;
	}
	return m_controllers[index];
}
