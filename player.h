/////////////////////////////
// player.h[プレイヤークラス]
//Author : hiroshi kasiwagi
//Date :2025/11/27
////////////////////////////////
#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "input_system.h"
#include "PlayerSystem.h"
#include "player_state.h"
#include "ObjectManager.h"
#include "controller.h"
#include "input_system.h"
#include "tps_camera.h"
#include "health.h"
#include "player_movement.h"
#include "player_morphsystem.h"
#include "player_command.h"


class Player : public GameObject
{
public:
    Player();
	~Player() = default;
};


#endif