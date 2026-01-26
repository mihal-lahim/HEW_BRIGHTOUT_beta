#include "Object.h"
#include "ObjectManager.h"

void Object::Instantiate(GameObject* obj)
{
	objectManager()->RegisterGameObject(obj);
}
