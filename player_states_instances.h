#ifndef PLAYER_STATES_INSTANCES_H
#define PLAYER_STATES_INSTANCES_H


#include "player_state_human_idle.h"
#include "player_state_human_walk.h"
#include "player_state_human_midair.h"

// ステートインスタンス格納構造体
struct PlayerStates
{
	inline static PlayerState_Human_Idle HumanIdle{};
	inline static PlayerState_Human_Walk HumanWalk{};
	inline static PlayerState_Human_MidAir HumanMidAir{};
};

#endif