/////////////////////////////////////////
//GamePad.cpp[コントローラー制御]
//Author: hiroshi kasiwagi
//Date: 2025/11/26
/////////////////////////////////////////

#include "GamePad.h"
#include <cmath>

#pragma comment(lib, "Xinput.lib")

GamePad::GamePad(int controllerNumber)
	: m_controllerNumber(controllerNumber),
	  m_prevState({}),
	  m_curState({}),
	  m_isConnected(false)
{}

void GamePad::InputUpdate()
{
	// 前回の状態を保存
	m_prevState = m_curState;
	
	// 前回のアナログ入力状態を保存
	m_prevLeftStick = GetLeftStick();
	m_prevRightStick = GetRightStick();
	m_prevLeftTrigger = GetLeftTrigger();
	m_prevRightTrigger = GetRightTrigger();

	// 現在の状態を取得
	XINPUT_STATE state{};
	DWORD result = XInputGetState(m_controllerNumber, &state);

	// 接続状態を更新
	m_isConnected = (result == ERROR_SUCCESS);

	// 接続されていない場合の処理
	if (!m_isConnected)
	{
		m_curState = {};
		return;
	}

	// 現在の状態を保存
	m_curState = state;
}

float GamePad::GetInputValue(InputKey input, InputCondition inputCondition)
{
	GamePadButton btn = static_cast<GamePadButton>(input);
	
	// アナログ入力の判定
	switch (btn)
	{
	case GamePadButton::TRIGGER_LEFT:
	case GamePadButton::TRIGGER_RIGHT:
		return GetTriggerValue(btn, inputCondition);

	case GamePadButton::STICK_LEFT_X:
	case GamePadButton::STICK_LEFT_Y:
	case GamePadButton::STICK_LEFT_LENGTH:
	case GamePadButton::STICK_RIGHT_X:
	case GamePadButton::STICK_RIGHT_Y:
	case GamePadButton::STICK_RIGHT_LENGTH:
		return GetStickValue(btn, inputCondition);

	default:
		// デジタルボタンの場合
		return GetButtonValue(btn, inputCondition);
	}
}


bool GamePad::IsDown(GamePadButton btn) const
{
	return (btn & m_curState.Gamepad.wButtons) && !(btn & m_prevState.Gamepad.wButtons);
}

bool GamePad::IsPressed(GamePadButton btn) const
{
	return (btn & m_curState.Gamepad.wButtons) && (btn & m_prevState.Gamepad.wButtons);
}

bool GamePad::IsReleased(GamePadButton btn) const
{
	return !(btn & m_curState.Gamepad.wButtons) && (btn & m_prevState.Gamepad.wButtons);
}


static constexpr float STICK_DIVIDER = 1.0f / 32767.0f;

StickState GamePad::GetLeftStick() const
{
	// 左スティックの状態
	StickState state{};

	// 取得
	float lx = (float)(m_curState.Gamepad.sThumbLX);
	float ly = (float)(m_curState.Gamepad.sThumbLY);

	// スティックの倒れ具合を計算
	float length = sqrtf(lx * lx + ly * ly);

	// デッドゾーン判定と正規化・値のセット
	state.IsOutside = length > m_leftStickDeadZone;
	state.Length = state.IsOutside ? length * STICK_DIVIDER : 0.0f;
	state.X = state.IsOutside ? lx * STICK_DIVIDER : 0.0f;
	state.Y = state.IsOutside ? ly * STICK_DIVIDER : 0.0f;

	return state;
}

StickState GamePad::GetRightStick() const
{
	// 右スティックの状態
	StickState state{};

	// 取得と正規化
	float rx = (float)(m_curState.Gamepad.sThumbRX);
	float ry = (float)(m_curState.Gamepad.sThumbRY);

	// スティックの倒れ具合を計算
	float length = sqrtf(rx * rx + ry * ry);

	// デッドゾーン判定と正規化・値のセット
	state.IsOutside = length > m_rightStickDeadZone;
	state.Length = state.IsOutside ? length * STICK_DIVIDER : 0.0f;
	state.X = state.IsOutside ? rx * STICK_DIVIDER : 0.0f;
	state.Y = state.IsOutside ? ry * STICK_DIVIDER : 0.0f;

	return state;
}


static constexpr float TRIGGER_DIVIDER = 1.0f / 255.0f;

TriggerState GamePad::GetLeftTrigger() const
{
	// 左トリガーの情報
	TriggerState state{};

	// 取得して正規化
	float value = (float)(m_curState.Gamepad.bLeftTrigger) * TRIGGER_DIVIDER;

	// 正規化した閾値と比較して値をセット、超えていなければ0.0f
	float normalizedThreshold = m_triggerThreshold * TRIGGER_DIVIDER;
	state.Value = value >= normalizedThreshold ? value : 0.0f;

	// 閾値を超えたかどうか
	state.IsDowned = value >= normalizedThreshold;

	return state;
}

TriggerState GamePad::GetRightTrigger() const
{
	// 右トリガーの情報
	TriggerState state{};

	// 取得して正規化
	float value = (float)(m_curState.Gamepad.bRightTrigger) * TRIGGER_DIVIDER;

	// 正規化した閾値と比較して値をセット、超えていなければ0.0f
	float normalizedThreshold = m_triggerThreshold * TRIGGER_DIVIDER;
	state.Value = value >= normalizedThreshold ? value : 0.0f;

	// 閾値を超えたかどうか
	state.IsDowned = value >= normalizedThreshold;

	return state;
}

float GamePad::GetButtonValue(GamePadButton btn, InputCondition inputCondition) const
{
	bool isCurrentPressed = (btn & m_curState.Gamepad.wButtons) != 0;
	bool isPrevPressed = (btn & m_prevState.Gamepad.wButtons) != 0;

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

float GamePad::GetTriggerValue(GamePadButton btn, InputCondition inputCondition) const
{
	TriggerState current{}, prev{};
	
	if (btn == GamePadButton::TRIGGER_LEFT)
	{
		current = GetLeftTrigger();
		prev = m_prevLeftTrigger;
	}
	else // TRIGGER_RIGHT
	{
		current = GetRightTrigger();
		prev = m_prevRightTrigger;
	}

	switch (inputCondition)
	{
	case InputCondition::STARTED:
		// 閾値を超えた瞬間
		return (current.IsDowned && !prev.IsDowned) ? current.Value : 0.0f;

	case InputCondition::PERFOMED:
		// 閾値を超えている間
		return (current.IsDowned && prev.IsDowned) ? current.Value : 0.0f;

	case InputCondition::CANCELED:
		// 閾値を下回った瞬間
		return (!current.IsDowned && prev.IsDowned) ? 1.0f : 0.0f;

	case InputCondition::NONE:
	default:
		return current.Value;
	}
}

float GamePad::GetStickValue(GamePadButton btn, InputCondition inputCondition) const
{
	StickState currentLeft = GetLeftStick();
	StickState currentRight = GetRightStick();
	
	StickState current{}, prev{};
	float value = 0.0f;

	// ボタンに応じて現在値と前回値を取得
	switch (btn)
	{
	case GamePadButton::STICK_LEFT_X:
		current = currentLeft;
		prev = m_prevLeftStick;
		value = current.X;
		break;

	case GamePadButton::STICK_LEFT_Y:
		current = currentLeft;
		prev = m_prevLeftStick;
		value = current.Y;
		break;

	case GamePadButton::STICK_LEFT_LENGTH:
		current = currentLeft;
		prev = m_prevLeftStick;
		value = current.Length;
		break;

	case GamePadButton::STICK_RIGHT_X:
		current = currentRight;
		prev = m_prevRightStick;
		value = current.X;
		break;

	case GamePadButton::STICK_RIGHT_Y:
		current = currentRight;
		prev = m_prevRightStick;
		value = current.Y;
		break;

	case GamePadButton::STICK_RIGHT_LENGTH:
		current = currentRight;
		prev = m_prevRightStick;
		value = current.Length;
		break;
	}

	switch (inputCondition)
	{
	case InputCondition::STARTED:
		// デッドゾーンの外に出た瞬間
		return (current.IsOutside && !prev.IsOutside) ? value : 0.0f;

	case InputCondition::PERFOMED:
		// デッドゾーンの外にいる間
		return (current.IsOutside && prev.IsOutside) ? value : 0.0f;

	case InputCondition::CANCELED:
		// デッドゾーンの内に入った瞬間
		return (!current.IsOutside && prev.IsOutside) ? 1.0f : 0.0f;

	case InputCondition::NONE:
	default:
		return value;
	}
}
