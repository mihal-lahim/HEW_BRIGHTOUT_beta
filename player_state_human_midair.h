#ifndef PLAYER_STATE_HUMAN_MIDAIR_H
#define PLAYER_STATE_HUMAN_MIDAIR_H

#include "player_state_human.h"


class PlayerState_Human_MidAir : public PlayerState_Human
{
public:
	void Enter() override;
	void HandleInput() override;
	void Update(double elapsedTime) override;
};

#endif
