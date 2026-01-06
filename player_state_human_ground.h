
#ifndef PLAYER_STATE_HUMAN_GROUND_H
#define PLAYER_STATE_HUMAN_GROUND_H


#include "player_state_human.h"


class PlayerState_Human_Ground : public PlayerState_Human
{
public:
	virtual void Enter(Player& player) override;
	virtual void HandleInput(Player& player) override;
	virtual void Update(Player& player, double elapsedTime) override;
	virtual void Draw(const Player& player) const override;
};


#endif
