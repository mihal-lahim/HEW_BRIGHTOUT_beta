/////////////////////////////////////////
//GamePad.h[コントローラー制御]
//Author: hiroshi kasiwagi
//Date: 2025/11/26
/////////////////////////////////////////

#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <windows.h>
#include <Xinput.h>
#include "InputDevice.h"

#pragma comment(lib, "Xinput.lib")

enum GamePadButton : WORD {
    BUTTON_A          = XINPUT_GAMEPAD_A,
    BUTTON_B          = XINPUT_GAMEPAD_B,
    BUTTON_X          = XINPUT_GAMEPAD_X,
    BUTTON_Y          = XINPUT_GAMEPAD_Y,
    BUTTON_LB         = XINPUT_GAMEPAD_LEFT_SHOULDER,
    BUTTON_RB         = XINPUT_GAMEPAD_RIGHT_SHOULDER,
    BUTTON_START      = XINPUT_GAMEPAD_START,
    BUTTON_BACK       = XINPUT_GAMEPAD_BACK,
    BUTTON_DPAD_UP    = XINPUT_GAMEPAD_DPAD_UP,
    BUTTON_DPAD_DOWN  = XINPUT_GAMEPAD_DPAD_DOWN,
    BUTTON_DPAD_LEFT  = XINPUT_GAMEPAD_DPAD_LEFT,
    BUTTON_DPAD_RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,

	TRIGGER_LEFT,
	TRIGGER_RIGHT,
	STICK_LEFT_X,
	STICK_LEFT_Y,
	STICK_RIGHT_X,
	STICK_RIGHT_Y,
    STICK_LEFT_LENGTH,
	STICK_RIGHT_LENGTH,
};

struct StickState {
    float X; // -1.0f ~ 1.0f
    float Y; // -1.0f ~ 1.0f
	float Length; // スティックの倒れ具合 (0.0f ~ 1.0f)
    bool IsOutside; // デッドゾーン外かどうか
};

struct TriggerState {
    float Value; // 0.0f ~ 1.0f
    bool IsDowned; // 閾値を超えたかどうか
};

class GamePad : public InputDevice
{
public:
	// コンストラクタ・デストラクタ
    GamePad(int controllerNumber = 0);
	~GamePad() = default;
    
    // 毎フレーム呼ぶ
    void InputUpdate();

	// 入力値取得（親クラスからオーバーライド）
	float GetInputValue(InputKey input, InputCondition inputCondition) override;
    
    // ボタン状態
    bool IsDown(GamePadButton btn) const;
    bool IsPressed(GamePadButton btn) const;
    bool IsReleased(GamePadButton btn) const;
    
    // スティック (-1.0f ~ 1.0f)
    StickState GetLeftStick() const;
    StickState GetRightStick() const;
    
    // トリガー (0.0f ~ 1.0f)
    TriggerState GetLeftTrigger() const;
    TriggerState GetRightTrigger() const;

	// 接続状態取得
	bool IsConnected() const { return m_isConnected; }

private:
	// ヘルパ関数
	float GetButtonValue(GamePadButton btn, InputCondition inputCondition) const;
	float GetTriggerValue(GamePadButton btn, InputCondition inputCondition) const;
	float GetStickValue(GamePadButton btn, InputCondition inputCondition) const;

	// コントローラー番号
    int m_controllerNumber;

	// 前回と今回の状態
    XINPUT_STATE m_prevState;
    XINPUT_STATE m_curState;

	// 前回のスティックとトリガーの状態（デッドゾーン判定後）
	mutable StickState m_prevLeftStick = {};
	mutable StickState m_prevRightStick = {};
	mutable TriggerState m_prevLeftTrigger = {};
	mutable TriggerState m_prevRightTrigger = {};

	// 接続状態
    bool m_isConnected = false;

	// スティックのデッドゾーン
	const float m_leftStickDeadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	const float m_rightStickDeadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

	// トリガーの反応閾値
    const float m_triggerThreshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
};


#endif
