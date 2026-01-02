

#ifndef PLAYER_COMMAND_H
#define PLAYER_COMMAND_H

#include "command.h"


// プレイヤーX軸移動コマンド
class PlayerCommand_MoveX : public Command {};

// プレイヤーZ軸移動コマンド
class PlayerCommand_MoveZ : public Command {};

// プレイヤージャンプコマンド
class PlayerCommand_Jump : public Command {};

// プレイヤー変身コマンド
class PlayerCommand_Morph : public Command {};


#endif
