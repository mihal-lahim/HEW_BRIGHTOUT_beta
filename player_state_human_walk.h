#ifndef PLAYER_STATE_HUMAN_WALK_H
#define PLAYER_STATE_HUMAN_WALK_H

#include "player_state_human_ground.h"


class PlayerState_Human_Walk : public PlayerState_Human_Ground
{
public:
	void Enter(PlayerSystem& playerSystem) override;
	void HandleInput(PlayerSystem& playerSystem) override;
	void Update(double elapsedTime, PlayerSystem& playerSystem) override;
};

#endif
