#ifndef PLAYER_STATE_HUMAN_MIDAIR_H
#define PLAYER_STATE_HUMAN_MIDAIR_H

#include "player_state_human.h"


class PlayerState_Human_MidAir : public PlayerState_Human
{
public:
	void Enter(Player& player) override;
	void HandleInput(Player& player) override;
	void Update(Player& player, double elapsedTime) override;
	void Draw(const Player& player) const override;
};

#endif
