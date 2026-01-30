/*==============================================================================

   キーボード入力 [KeyLogger.h]
														 Author : sumi rintarou
														 Date   : 2025/06/27
--------------------------------------------------------------------------------

==============================================================================*/

#ifndef KEY_LOGGER_H
#define KEY_LOGGER_H
#include "keyboard.h"

void KeyLogger_Initialize();
void KeyLogger_Update();
bool KeyLogger_IsPressed(KeyboardKeys key);
bool KeyLogger_IsTrigger(KeyboardKeys key);
bool KeyLogger_IsRelease(KeyboardKeys key);


#endif // KEY_LOGGER_H
