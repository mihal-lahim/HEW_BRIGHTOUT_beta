#include "player_morphsystem.h"

bool PlayerMorphSystem::CanMorph(const Player&)
{
    return false;
}

PowerLineID PlayerMorphSystem::GetNearestPowerLineID(const Player&)
{
    return PowerLineID();
}
