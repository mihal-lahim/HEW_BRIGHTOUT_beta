
#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include "Keyboard.h"
#include "GamePad.h"
#include <array>

class EngineCore;
class Scene;

enum class InputDeviceType
{
	KEYBOARD,
	GAMEPAD,
};

class InputSystem
{
public:
	InputSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{ Initialize(); }
	~InputSystem() { Finalize(); }

	// 入力システムの初期化
	void Initialize();
	// 入力システムの更新
	void Update();
	// 入力システムの終了処理
	void Finalize();

	// キーボードデバイス取得
	Keyboard& keyboard() { return m_keyboard; }

	// キーボードデバイス取得
	GamePad& gamePad(int index = 0) { return m_gamePads.at(index); }

private:
	// 所属するエンジンコア
	EngineCore* m_engineCore = nullptr;

	// キーボードデバイス
	Keyboard m_keyboard{};

	// ゲームパッドデバイス（最大4つまで対応）
	std::array<GamePad, 4> m_gamePads{ GamePad(0), GamePad(1), GamePad(2), GamePad(3) };
};


#endif
