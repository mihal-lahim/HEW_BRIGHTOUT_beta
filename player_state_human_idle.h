#pragma once

#ifndef PLAYER_STATE_HUMAN_IDLE_H
#define PLAYER_STATE_HUMAN_IDLE_H

#include "player_state_human_ground.h"


class PlayerState_Human_Idle : public PlayerState_Human_Ground
{
public:
	void Enter(Player& player) override;
	void HandleInput(Player& player) override;
	void Update(Player& player, double elapsedTime) override;
	void Draw(const Player& player) const override;
	void Exit(Player& player) override;
};

#endif
