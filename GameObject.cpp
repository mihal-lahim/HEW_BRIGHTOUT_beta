#include "GameObject.h"
#include "ObjectManager.h"


GameObject::GameObject()
{
	objectManager()->RegisterGameObject(this);
}

void GameObject::RegisterComponent(Component* comp)
{
	comp->m_GameObject = this;
	objectManager()->RegisterComponent(comp);
}
