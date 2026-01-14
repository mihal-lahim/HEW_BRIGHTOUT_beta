
#ifndef PLAYER_MORPHSYSTEM_H
#define PLAYER_MORPHSYSTEM_H

#include "Component.h"
#include "PoleManager.h"

class Player;

class PlayerMorphSystem : public Component
{
private:

	PoleManager* m_PoleManager = nullptr;
public:
	void Start() override;

	// •Ïg‰Â”\‚©‚Ç‚¤‚©‚ğ”»’è‚·‚é
	bool CanMorph();

	// ÅŠñ‚è‚Ì“düID‚ğæ“¾‚·‚é
	PowerLineID GetNearestPowerLineID();
};


#endif
