

#ifndef PLAYER_COMMAND_SET_H
#define PLAYER_COMMAND_SET_H

#include "command_set.h"
#include "player_command.h"
#include "controller.h"

// プレイヤー用コマンドセット
class PlayerCommandSet : public CommandSet
{
public:
	PlayerCommandSet()
	{
		// 移動コマンドを登録
		RegisterCommand<XBOXButton, PlayerCommand_MoveX>(STICK_LEFT_X, InputCondition::PERFOMED);
		RegisterCommand<XBOXButton, PlayerCommand_MoveZ>(STICK_LEFT_Y, InputCondition::PERFOMED);

		// カメラ移動コマンドを登録
		RegisterCommand<XBOXButton, PlayerCommand_CameraMoveX>(STICK_RIGHT_X, InputCondition::PERFOMED);
		RegisterCommand<XBOXButton, PlayerCommand_CameraMoveY>(STICK_RIGHT_Y, InputCondition::PERFOMED);

		// ジャンプコマンドを設定
		RegisterCommand<XBOXButton, PlayerCommand_Jump>(BUTTON_A, InputCondition::STARTED);
		// 変身コマンドを許可
		RegisterCommand<XBOXButton, PlayerCommand_Morph>(BUTTON_X, InputCondition::STARTED);

		// ADSコマンドを登録
		RegisterCommand<XBOXButton, playerCommand_ADS>(TRIGGER_LEFT, InputCondition::PERFOMED);
		// 射撃コマンドを登録
		RegisterCommand<XBOXButton, PlayerCommand_Fire>(TRIGGER_RIGHT, InputCondition::STARTED);
	}

	virtual ~PlayerCommandSet()
	{}
};



#endif
