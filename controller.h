/////////////////////////////////////////
//controller.h[コントローラー制御]
//Author: hiroshi kasiwagi
//Date: 2025/11/26
/////////////////////////////////////////
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <windows.h>
#include <Xinput.h>
#include "input_device.h"

#pragma comment(lib, "Xinput.lib")

enum Button : WORD {
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
    float x; // -1.0f ~ 1.0f
    float y; // -1.0f ~ 1.0f
	float length; // スティックの倒れ具合 (0.0f ~ 1.0f)
    bool isOutside; // デッドゾーン外かどうか
};

struct TriggerState {
    float value; // 0.0f ~ 1.0f
    bool isDowned; // 閾値を超えたかどうか
};

class Controller : public InputDevice
{
public:
	// コンストラクタ・デストラクタ
    Controller(int controllerNumber = 0);
	~Controller() = default;
    
    // 毎フレーム呼ぶ
    void PreUpdate();

	// 入力値取得（親クラスからオーバーライド）
	float GetInputValue(InputKey input, InputCondition inputCondition) override;
    
    // ボタン状態
    bool IsDown(Button btn) const;
    bool IsPressed(Button btn) const;
    bool IsReleased(Button btn) const;
    
    // スティック (-1.0f ~ 1.0f)
    StickState GetLeftStick() const;
    StickState GetRightStick() const;
    
    // トリガー (0.0f ~ 1.0f)
    TriggerState GetLeftTrigger() const;
    TriggerState GetRightTrigger() const;

	// 接続状態取得
	bool IsConnected() const { return m_IsConnected; }

private:
	// ヘルパ関数
	float GetButtonValue(Button btn, InputCondition inputCondition) const;
	float GetTriggerValue(Button btn, InputCondition inputCondition) const;
	float GetStickValue(Button btn, InputCondition inputCondition) const;

	// コントローラー番号
    int m_ControllerNumber;

	// 前回と今回の状態
    XINPUT_STATE m_PrevState;
    XINPUT_STATE m_CurState;

	// 前回のスティックとトリガーの状態（デッドゾーン判定後）
	mutable StickState m_PrevLeftStick = {};
	mutable StickState m_PrevRightStick = {};
	mutable TriggerState m_PrevLeftTrigger = {};
	mutable TriggerState m_PrevRightTrigger = {};

	// 接続状態
    bool m_IsConnected = false;

	// スティックのデッドゾーン
	const float m_LeftStickDeadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	const float m_RightStickDeadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

	// トリガーの反応閾値
	const float m_TriggerThreshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
};


#endif
