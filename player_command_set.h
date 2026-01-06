

#ifndef PLAYER_COMMAND_SET_H
#define PLAYER_COMMAND_SET_H

#include "command_set.h"
#include "player_command.h"

// プレイヤー用コマンドセット基底クラス
class PlayerComSet : public CommandSet
{
public:
	PlayerComSet()
	{}

	virtual ~PlayerComSet()
	{}
};

// 人間形態のコマンドセット
class PlayerComSet_Human : public PlayerComSet
{
public:
	PlayerComSet_Human()
	{
		// 移動コマンドを許可
		AllowCommand<PlayerCommand_MoveX>();
		AllowCommand<PlayerCommand_MoveZ>();

		// ジャンプコマンドを許可
		AllowCommand<PlayerCommand_Jump>();
		// 変身コマンドを許可
		AllowCommand<PlayerCommand_Morph>();
		// 電気ジャンプコマンドを許可
		AllowCommand<PlayerCommand_Jump>();
	}

	virtual ~PlayerComSet_Human()
	{}
};



#endif
