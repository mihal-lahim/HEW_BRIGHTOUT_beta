#ifndef PLAYER_STATE_HUMAN_H
#define PLAYER_STATE_HUMAN_H

#include "PlayerState.h"


class PlayerState_Human : public PlayerState
{
public:
	virtual void Enter(Player& player) override;
	virtual void HandleInput(Player& player) override;
	virtual void Update(Player& player) override;
};

#endif
