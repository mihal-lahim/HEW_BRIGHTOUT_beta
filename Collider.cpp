#include "Collider.h"

void Collider::SetActive(bool isActive)
{
	if (m_IsStatic && IsActive() && !isActive)
	{
		m_PhysicsSystem->UnregisterCollider(this);
	}
	else if(m_IsStatic && !IsActive() && isActive)
	{
		m_PhysicsSystem->RegisterCollider(this);
	}

	Component::SetActive(isActive);
}
