#ifndef PLAYER_COMMAND_SET_H
#define PLAYER_COMMAND_SET_H

#include "CommandSet.h"
#include "PlayerCommand.h"
#include "GamePad.h"

// プレイヤー用コマンドセット
class PlayerCommandSet : public CommandSet
{
public:
	PlayerCommandSet()
	{
		// 移動コマンドを登録
		RegisterCommand<GamePadButton, PlayerCommand_MoveX>(STICK_LEFT_X, InputCondition::PERFOMED);
		RegisterCommand<GamePadButton, PlayerCommand_MoveZ>(STICK_LEFT_Y, InputCondition::PERFOMED);

		// カメラ移動コマンドを登録
		RegisterCommand<GamePadButton, PlayerCommand_CameraMoveX>(STICK_RIGHT_X, InputCondition::PERFOMED);
		RegisterCommand<GamePadButton, PlayerCommand_CameraMoveY>(STICK_RIGHT_Y, InputCondition::PERFOMED);

		// ジャンプコマンドを設定
		RegisterCommand<GamePadButton, PlayerCommand_Jump>(BUTTON_A, InputCondition::STARTED);
		// 変身コマンドを許可
		RegisterCommand<GamePadButton, PlayerCommand_Morph>(BUTTON_X, InputCondition::STARTED);

		// ADSコマンドを登録
		RegisterCommand<GamePadButton, playerCommand_ADS>(TRIGGER_LEFT, InputCondition::PERFOMED);
		// 射撃コマンドを登録
		RegisterCommand<GamePadButton, PlayerCommand_Fire>(TRIGGER_RIGHT, InputCondition::STARTED);

		// インタラクトコマンドを登録
		RegisterCommand<GamePadButton, PlayerCommand_Interact>(BUTTON_B, InputCondition::NONE);
	}

	virtual ~PlayerCommandSet()
	{}
};



#endif
