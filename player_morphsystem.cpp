#include "player_morphsystem.h"
#include "ObjectManager.h"

bool PlayerMorphSystem::CanMorph()
{
    return false;
}

PowerLineID PlayerMorphSystem::GetNearestPowerLineID()
{
    return PowerLineID();
}

void PlayerMorphSystem::Start()
{
	// PoleManagerŽæ“¾
	m_PoleManager = GetOwner()->GetOwner()->GetGameObject<PoleManager>();
}
