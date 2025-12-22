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
