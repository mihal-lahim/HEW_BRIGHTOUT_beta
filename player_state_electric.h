



#ifndef PLAYER_STATE_ELECTRIC_H
#define PLAYER_STATE_ELECTRIC_H

#include "player_state.h"

class PlayerState_Electric : public PlayerState
{
public:
	virtual void Enter(PlayerSystem& playerSystem) override;
	virtual void HandleInput(PlayerSystem& playerSystem) override;
	virtual void Update(double elapsedTime, PlayerSystem& playerSystem) override;
};


#endif
