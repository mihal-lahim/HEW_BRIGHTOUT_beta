
#ifndef PLAYER_STATE_ELECTRIC_H
#define PLAYER_STATE_ELECTRIC_H

#include "player_state.h"

class PlayerState_Electric : public PlayerState
{
public:
	virtual void Enter() override;
	virtual void HandleInput() override;
	virtual void Update(double elapsedTime) override;
};


#endif
