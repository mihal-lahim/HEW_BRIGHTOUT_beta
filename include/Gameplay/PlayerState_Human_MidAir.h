#ifndef PLAYER_STATE_HUMAN_MIDAIR_H
#define PLAYER_STATE_HUMAN_MIDAIR_H

#include "PlayerState_Human.h"

class Player;

class PlayerState_Human_MidAir : public PlayerState_Human
{
public:
	void Enter(Player& player) override;
	void HandleInput(Player& player) override;
	void Update(Player& player) override;
};

#endif
