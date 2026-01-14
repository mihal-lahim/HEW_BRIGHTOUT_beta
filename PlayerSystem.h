
#ifndef PLAYER_SYSTEM_H
#define PLAYER_SYSTEM_H

#include "Component.h"

class PlayerStateMachine;

class PlayerSystem : public Component
{
private:
	// èÛë‘ä«óù
	PlayerStateMachine* m_StateMachine = nullptr;
	
public:
	void Start() override;
	void Update(double elapsedTime) override;
};



#endif