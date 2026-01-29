#include "Component.h"
#include "GameObject.h"

bool Component::IsActive() const
{
	return m_GameObject->IsActive();
}

void Component::SetActive(bool active)
{
}
