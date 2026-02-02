//--------------------------------------------------------------------------------------
// File: Keyboard.cpp
//
// キーボードモジュール
//
//--------------------------------------------------------------------------------------
// 2020/06/07
//     DirectXTKより、なんちゃってC言語用にシェイプアップ改変
//
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------
#include "Keyboard.h"

#include <assert.h>


static_assert(sizeof(KeyboardState) == 256 / 8, "キーボード状態構造体のサイズ不一致");


static KeyboardState gState = {};


static void keyDown(int key)
{
	if (key < 0 || key > 0xfe) { return; }

	unsigned int* p = (unsigned int*)&gState;
	unsigned int bf = 1u << (key & 0x1f);
	p[(key >> 5)] |= bf;
}


static void keyUp(int key)
{
	if (key < 0 || key > 0xfe) { return; }

	unsigned int* p = (unsigned int*)&gState;
	unsigned int bf = 1u << (key & 0x1f);
	p[(key >> 5)] &= ~bf;
}


void Keyboard_Initialize(void)
{
	Keyboard_Reset();
}


bool Keyboard_IsKeyDown(KeyboardKeys key, const KeyboardState* pState)
{
	if (key <= 0xfe)
	{
		unsigned int* p = (unsigned int*)pState;
		unsigned int bf = 1u << (key & 0x1f);
		return (p[(key >> 5)] & bf) != 0;
	}
	return false;
}


bool Keyboard_IsKeyUp(KeyboardKeys key, const KeyboardState* pState)
{
	if (key <= 0xfe)
	{
		unsigned int* p = (unsigned int*)pState;
		unsigned int bf = 1u << (key & 0x1f);
		return (p[(key >> 5)] & bf) == 0;
	}
	return false;
}


bool Keyboard_IsKeyDown(KeyboardKeys key)
{
	return Keyboard_IsKeyDown(key, &gState);
}


bool Keyboard_IsKeyUp(KeyboardKeys key)
{
	return Keyboard_IsKeyUp(key, &gState);
}


// キーボードの現在の状態を取得する
const KeyboardState* Keyboard_GetState(void)
{
	return &gState;
}


void Keyboard_Reset(void)
{
	ZeroMemory(&gState, sizeof(KeyboardState));
}


// キーボード制御のためのウォンどうメッセージプロシージャフック関数
void Keyboard_ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	bool down = false;

	switch (message)
	{
	case WM_ACTIVATEAPP:
		Keyboard_Reset();
		return;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		down = true;
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		break;

	default:
		return;
	}

	int vk = (int)wParam;
	switch (vk)
	{
	case VK_SHIFT:
		vk = (int)MapVirtualKey(((unsigned int)lParam & 0x00ff0000) >> 16u, MAPVK_VSC_TO_VK_EX);
		if (!down)
		{
			// 左シフトと右シフトの両方が同時に押された場合にクリアされるようにするための回避策
			keyUp(VK_LSHIFT);
			keyUp(VK_RSHIFT);
		}
		break;

	case VK_CONTROL:
		vk = ((UINT)lParam & 0x01000000) ? VK_RCONTROL : VK_LCONTROL;
		break;

	case VK_MENU:
		vk = ((UINT)lParam & 0x01000000) ? VK_RMENU : VK_LMENU;
		break;
	}

	if (down)
	{
		keyDown(vk);
	}
	else
	{
		keyUp(vk);
	}
}


// Keyboardクラスの実装

Keyboard::Keyboard()
    : m_PrevState({}),
      m_CurState({}),
      m_NextState({})
{
}

void Keyboard::InputUpdate()
{
    // 前回の状態を保存
    m_PrevState = m_CurState;
    m_CurState = m_NextState;
}

float Keyboard::GetInputValue(InputKey input, InputCondition inputCondition)
{
	KeyboardKeys key = static_cast<KeyboardKeys>(input);

	switch (inputCondition)
	{
	case InputCondition::STARTED:
		return IsDown(key) ? 1.0f : 0.0f;

	case InputCondition::PERFOMED:
		return IsPressed(key) ? 1.0f : 0.0f;

	case InputCondition::CANCELED:
		return IsReleased(key) ? 1.0f : 0.0f;

	default:
		return 0.0f;
	}
}

bool Keyboard::IsDown(KeyboardKeys key) const
{
	return IsKeyInState(key, &m_CurState) && !IsKeyInState(key, &m_PrevState);
}

bool Keyboard::IsPressed(KeyboardKeys key) const
{
	return IsKeyInState(key, &m_CurState) && IsKeyInState(key, &m_PrevState);
}

bool Keyboard::IsReleased(KeyboardKeys key) const
{
	return !IsKeyInState(key, &m_CurState) && IsKeyInState(key, &m_PrevState);
}

void Keyboard::Reset()
{
	ZeroMemory(&m_PrevState, sizeof(KeyboardState));
	ZeroMemory(&m_CurState, sizeof(KeyboardState));
	ZeroMemory(&m_NextState, sizeof(KeyboardState));
}

void Keyboard::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	bool down = false;

	switch (message)
	{
	case WM_ACTIVATEAPP:
		Reset();
		return;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		down = true;
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		break;

	default:
		return;
	}

	int vk = (int)wParam;
	switch (vk)
	{
	case VK_SHIFT:
		vk = (int)MapVirtualKey(((unsigned int)lParam & 0x00ff0000) >> 16u, MAPVK_VSC_TO_VK_EX);
		if (!down)
		{
			// 左シフトと右シフトの両方が同時に押された場合にクリアされるようにするための回避策
			KeyUp(VK_LSHIFT);
			KeyUp(VK_RSHIFT);
		}
		break;

	case VK_CONTROL:
		vk = ((UINT)lParam & 0x01000000) ? VK_RCONTROL : VK_LCONTROL;
		break;

	case VK_MENU:
		vk = ((UINT)lParam & 0x01000000) ? VK_RMENU : VK_LMENU;
		break;
	}

	if (down)
	{
		KeyDown(vk);
	}
	else
	{
		KeyUp(vk);
	}
}

void Keyboard::KeyDown(int key)
{
	if (key < 0 || key > 0xfe) { return; }

	unsigned int* p = (unsigned int*)&m_NextState;
	unsigned int bf = 1u << (key & 0x1f);
	p[(key >> 5)] |= bf;
}

void Keyboard::KeyUp(int key)
{
	if (key < 0 || key > 0xfe) { return; }

	unsigned int* p = (unsigned int*)&m_NextState;
	unsigned int bf = 1u << (key & 0x1f);
	p[(key >> 5)] &= ~bf;
}

bool Keyboard::IsKeyInState(KeyboardKeys key, const KeyboardState* pState) const
{
	if (key <= 0xfe)
	{
		unsigned int* p = (unsigned int*)pState;
		unsigned int bf = 1u << (key & 0x1f);
		return (p[(key >> 5)] & bf) != 0;
	}
	return false;
}
