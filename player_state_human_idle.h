#pragma once

#ifndef PLAYER_STATE_HUMAN_IDLE_H
#define PLAYER_STATE_HUMAN_IDLE_H

#include "player_state_human_ground.h"


class PlayerState_Human_Idle : public PlayerState_Human_Ground
{
public:
	void Enter() override;
	void HandleInput() override;
	void Update(double elapsedTime) override;
};

#endif
