







#ifndef PLAYER_STATE_ELECTRIC_H
#define PLAYER_STATE_ELECTRIC_H

#include "PlayerState.h"

class PlayerState_Electric : public PlayerState
{
public:
	virtual void Enter(Player& player) override;
	virtual void HandleInput(Player& player) override;
	virtual void Update(Player& player) override;
};


#endif
