#include "GameObject.h"
#include "ObjectManager.h"


void GameObject::RegisterComponent(Component* comp)
{
	comp->m_GameObject = this;
	objectManager()->RegisterComponent(comp);
}
