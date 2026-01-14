#ifndef PLAYER_STATE_HUMAN_H
#define PLAYER_STATE_HUMAN_H

#include "player_state.h"


class PlayerState_Human : public PlayerState
{
public:
	virtual void Enter(PlayerSystem& playerSystem) override;
	virtual void HandleInput(PlayerSystem& playerSystem) override;
	virtual void Update(double elapsedTime, PlayerSystem& playerSystem) override;
};

#endif
