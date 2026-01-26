#include "PlayerMorphsystem.h"
#include "ObjectManager.h"

void PlayerMorphSystem::Awake()
{
    // PoleManager‚Ö‚ÌŽQÆ‚ðŽæ“¾
	m_PoleManager = objectManager()->GetGameObject<PoleManager>();
}

bool PlayerMorphSystem::CanMorph()
{
    return false;
}

PowerLineID PlayerMorphSystem::GetNearestPowerLineID()
{
    return PowerLineID();
}