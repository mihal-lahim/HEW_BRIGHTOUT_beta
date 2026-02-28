#pragma once

#ifndef PLAYER_STATE_HUMAN_IDLE_H
#define PLAYER_STATE_HUMAN_IDLE_H

#include "PlayerState_Human_Ground.h"


class PlayerState_Human_Idle : public PlayerState_Human_Ground
{
public:
	void Enter(Player& player) override;
	void HandleInput(Player& player) override;
	void Update(Player& player) override;
};

#endif
