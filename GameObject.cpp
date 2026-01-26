#include "GameObject.h"
#include "ObjectManager.h"


void GameObject::RegisterComponent(Component* comp)
{
	objectManager()->RegisterComponent(comp);
}
