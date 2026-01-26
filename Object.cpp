#include "Object.h"
#include "GameManager.h"

Object::Object()
	: m_ObjectManager(&GameManager::GetObjectManager())
{}
