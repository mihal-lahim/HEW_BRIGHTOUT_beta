
#ifndef PLAYER_MORPHSYSTEM_H
#define PLAYER_MORPHSYSTEM_H

#include "GameObject.h"
#include "PoleManager.h"

class Player;

class PlayerMorphSystem : public GameObject
{
public:
	// •Ïg‰Â”\‚©‚Ç‚¤‚©‚ğ”»’è‚·‚é
	bool CanMorph(const Player& player);

	// ÅŠñ‚è‚Ì“düID‚ğæ“¾‚·‚é
	PowerLineID GetNearestPowerLineID(const Player& player);
};


#endif
