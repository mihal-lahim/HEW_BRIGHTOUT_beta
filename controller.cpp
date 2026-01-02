/////////////////////////////////////////
//controller.cpp[コントローラー制御]
//Author: hiroshi kasiwagi
//Date: 2025/11/26
/////////////////////////////////////////

#include "controller.h"
#include <cmath>

#pragma comment(lib, "Xinput.lib")

Controller::Controller(int controllerNumber)
	: m_ControllerNumber(controllerNumber),
	  m_PrevState({}),
	  m_CurState({}),
	  m_IsConnected(false)
{}

void Controller::Update()
{
	// 前回の状態を保存
	m_PrevState = m_CurState;
	
	// 前回のアナログ入力状態を保存
	m_PrevLeftStick = GetLeftStick();
	m_PrevRightStick = GetRightStick();
	m_PrevLeftTrigger = GetLeftTrigger();
	m_PrevRightTrigger = GetRightTrigger();

	// 現在の状態を取得
	XINPUT_STATE state{};
	DWORD result = XInputGetState(m_ControllerNumber, &state);

	// 接続状態を更新
	m_IsConnected = (result == ERROR_SUCCESS);

	// 接続されていない場合の処理
	if (!m_IsConnected)
	{
		m_CurState = {};
		return;
	}

	// 現在の状態を保存
	m_CurState = state;
}

float Controller::GetInputValue(InputKey input, InputCondition inputCondition)
{
	Button btn = static_cast<Button>(input);
	
	// アナログ入力の判定
	switch (btn)
	{
	case Button::TRIGGER_LEFT:
	case Button::TRIGGER_RIGHT:
		return GetTriggerValue(btn, inputCondition);

	case Button::STICK_LEFT_X:
	case Button::STICK_LEFT_Y:
	case Button::STICK_LEFT_LENGTH:
	case Button::STICK_RIGHT_X:
	case Button::STICK_RIGHT_Y:
	case Button::STICK_RIGHT_LENGTH:
		return GetStickValue(btn, inputCondition);

	default:
		// デジタルボタンの場合
		return GetButtonValue(btn, inputCondition);
	}
}


bool Controller::IsDown(Button btn) const
{
	return (btn & m_CurState.Gamepad.wButtons) && !(btn & m_PrevState.Gamepad.wButtons);
}

bool Controller::IsPressed(Button btn) const
{
	return (btn & m_CurState.Gamepad.wButtons) && (btn & m_PrevState.Gamepad.wButtons);
}

bool Controller::IsReleased(Button btn) const
{
	return !(btn & m_CurState.Gamepad.wButtons) && (btn & m_PrevState.Gamepad.wButtons);
}


static constexpr float STICK_DIVIDER = 1.0f / 32767.0f;

StickState Controller::GetLeftStick() const
{
	// 左スティックの状態
	StickState state{};

	// 取得
	float lx = (float)(m_CurState.Gamepad.sThumbLX);
	float ly = (float)(m_CurState.Gamepad.sThumbLY);

	// スティックの倒れ具合を計算
	float length = sqrtf(lx * lx + ly * ly);

	// デッドゾーン判定と正規化・値のセット
	state.isOutside = length > m_LeftStickDeadZone;
	state.length = state.isOutside ? length * STICK_DIVIDER : 0.0f;
	state.x = state.isOutside * STICK_DIVIDER ? lx : 0.0f;
	state.y = state.isOutside * STICK_DIVIDER ? ly : 0.0f;

	return state;
}

StickState Controller::GetRightStick() const
{
	// 右スティックの状態
	StickState state{};

	// 取得と正規化
	float rx = (float)(m_CurState.Gamepad.sThumbRX);
	float ry = (float)(m_CurState.Gamepad.sThumbRY);

	// スティックの倒れ具合を計算
	float length = sqrtf(rx * rx + ry * ry);

	// デッドゾーン判定と値のセット
	state.isOutside = length > (m_LeftStickDeadZone * STICK_DIVIDER);
	state.length = state.isOutside ? length * STICK_DIVIDER : 0.0f;
	state.x = state.isOutside * STICK_DIVIDER ? rx : 0.0f;
	state.y = state.isOutside * STICK_DIVIDER ? ry : 0.0f;

	return state;
}


static constexpr float TRIGGER_DIVIDER = 1.0f / 255.0f;

TriggerState Controller::GetLeftTrigger() const
{
	// 左トリガーの状態
	TriggerState state{};

	// 取得して正規化
	float value = (float)(m_CurState.Gamepad.bLeftTrigger) * TRIGGER_DIVIDER;

	// 閾値を超えていれば値をセット、超えていなければ0.0f
	state.value = value >= m_TriggerThreshold ? value : 0.0f;

	// 閾値を超えたかどうか
	state.isDowned = m_CurState.Gamepad.bLeftTrigger > m_TriggerThreshold;

	return state;
}

TriggerState Controller::GetRightTrigger() const
{
	// 右トリガーの状態
	TriggerState state{};

	// 取得して正規化
	float value = (float)(m_CurState.Gamepad.bRightTrigger) * TRIGGER_DIVIDER;

	// 閾値を超えていれば値をセット、超えていなければ0.0f
	state.value = value >= m_TriggerThreshold ? value : 0.0f;

	// 閾値を超えたかどうか
	state.isDowned = m_CurState.Gamepad.bLeftTrigger > m_TriggerThreshold;

	return state;
}

float Controller::GetButtonValue(Button btn, InputCondition inputCondition) const
{
	bool isCurrentPressed = (btn & m_CurState.Gamepad.wButtons) != 0;
	bool isPrevPressed = (btn & m_PrevState.Gamepad.wButtons) != 0;

	switch (inputCondition)
	{
	case InputCondition::STARTED:
		return (isCurrentPressed && !isPrevPressed) ? 1.0f : 0.0f;

	case InputCondition::PERFOMED:
		return (isCurrentPressed && isPrevPressed) ? 1.0f : 0.0f;

	case InputCondition::CANCELED:
		return (!isCurrentPressed && isPrevPressed) ? 1.0f : 0.0f;

	case InputCondition::NONE:
	default:
		return isCurrentPressed ? 1.0f : 0.0f;
	}
}

float Controller::GetTriggerValue(Button btn, InputCondition inputCondition) const
{
	TriggerState current{}, prev{};
	
	if (btn == Button::TRIGGER_LEFT)
	{
		current = GetLeftTrigger();
		prev = m_PrevLeftTrigger;
	}
	else // TRIGGER_RIGHT
	{
		current = GetRightTrigger();
		prev = m_PrevRightTrigger;
	}

	switch (inputCondition)
	{
	case InputCondition::STARTED:
		// 閾値を超えた瞬間
		return (current.isDowned && !prev.isDowned) ? current.value : 0.0f;

	case InputCondition::PERFOMED:
		// 閾値を超えている間
		return (current.isDowned && prev.isDowned) ? current.value : 0.0f;

	case InputCondition::CANCELED:
		// 閾値を下回った瞬間
		return (!current.isDowned && prev.isDowned) ? 1.0f : 0.0f;

	case InputCondition::NONE:
	default:
		return current.value;
	}
}

float Controller::GetStickValue(Button btn, InputCondition inputCondition) const
{
	StickState currentLeft = GetLeftStick();
	StickState currentRight = GetRightStick();
	
	StickState current{}, prev{};
	float value = 0.0f;

	// ボタンに応じて現在値と前回値を取得
	switch (btn)
	{
	case Button::STICK_LEFT_X:
		current = currentLeft;
		prev = m_PrevLeftStick;
		value = current.x;
		break;

	case Button::STICK_LEFT_Y:
		current = currentLeft;
		prev = m_PrevLeftStick;
		value = current.y;
		break;

	case Button::STICK_LEFT_LENGTH:
		current = currentLeft;
		prev = m_PrevLeftStick;
		value = current.length;
		break;

	case Button::STICK_RIGHT_X:
		current = currentRight;
		prev = m_PrevRightStick;
		value = current.x;
		break;

	case Button::STICK_RIGHT_Y:
		current = currentRight;
		prev = m_PrevRightStick;
		value = current.y;
		break;

	case Button::STICK_RIGHT_LENGTH:
		current = currentRight;
		prev = m_PrevRightStick;
		value = current.length;
		break;
	}

	switch (inputCondition)
	{
	case InputCondition::STARTED:
		// デッドゾーンの外に出た瞬間
		return (current.isOutside && !prev.isOutside) ? value : 0.0f;

	case InputCondition::PERFOMED:
		// デッドゾーンの外にいる間
		return (current.isOutside && prev.isOutside) ? value : 0.0f;

	case InputCondition::CANCELED:
		// デッドゾーンの内に入った瞬間
		return (!current.isOutside && prev.isOutside) ? 1.0f : 0.0f;

	case InputCondition::NONE:
	default:
		return value;
	}
}
