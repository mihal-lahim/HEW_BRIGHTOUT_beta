#ifndef PLAYER_STATE_HUMAN_H
#define PLAYER_STATE_HUMAN_H

#include "player_state.h"


class PlayerState_Human : public PlayerState
{
public:
	virtual void Enter() override;
	virtual void HandleInput() override;
	virtual void Update(double elapsedTime) override;
};

#endif
