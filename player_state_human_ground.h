#ifndef PLAYER_STATE_HUMAN_GROUND_H
#define PLAYER_STATE_HUMAN_GROUND_H


#include "player_state_human.h"


class PlayerState_Human_Ground : public PlayerState_Human
{
public:
	virtual void Enter(PlayerSystem& playerSystem) override;
	virtual void HandleInput(PlayerSystem& playerSystem) override;
	virtual void Update(double elapsedTime, PlayerSystem& playerSystem) override;
};


#endif
