#include "Component.h"
#include "GameObject.h"

bool Component::IsActive() const
{
	{ return Object::IsActive() && m_GameObject->IsActive(); }
}
