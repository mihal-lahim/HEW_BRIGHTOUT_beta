/*==============================================================================

   キーボード入力の記録 [KeyLogger.cpp]
														 Author : sumi rintarou
														 Date   : 2025/06/27
--------------------------------------------------------------------------------

==============================================================================*/

#include "KeyLogger.h"

static KeyboardState g_PrevState = {};
static KeyboardState g_TriggerState = {};
static KeyboardState g_ReleaseState = {};

void KeyLogger_Initialize()
{
	Keyboard_Initialize();
}

void KeyLogger_Update()
{
	const KeyboardState* state = Keyboard_GetState();
	LPBYTE pt = (LPBYTE)&g_TriggerState;
	LPBYTE pn = (LPBYTE)state;
	LPBYTE po = (LPBYTE)&g_PrevState;
	LPBYTE pr = (LPBYTE)&g_ReleaseState;

	for (int i = 0; i < sizeof(KeyboardState); i++)
	{
		pt[i] = (po[i] ^ pn[i]) & pn[i];
		pr[i] = (po[i] ^ pn[i]) & ~pn[i];
		//pr[i] = po[i] & ~pn[i];
	}
	g_PrevState = *state;
}

bool KeyLogger_IsPressed(KeyboardKeys key)
{
	return Keyboard_IsKeyDown(key);
}

bool KeyLogger_IsTrigger(KeyboardKeys key)
{
	return Keyboard_IsKeyDown(key, &g_TriggerState);
}

bool KeyLogger_IsRelease(KeyboardKeys key)
{
	return Keyboard_IsKeyDown(key, &g_ReleaseState);
}
