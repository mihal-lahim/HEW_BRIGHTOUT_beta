
#include "ObjectManager.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "GameManager.h"


Scene* const GameManager::GetCurrentScene()
{ 
	return CurrentScene.get(); 
}


PhysicsSystem& GameManager::GetPhysicsSystem() 
{ 
	return CurrentScene->GetPhysicsSystem(); 
}


ObjectManager& GameManager::GetObjectManager() 
{ 
	return CurrentScene->GetObjectManager(); 
}


RenderSystem& GameManager::GetRenderSystem() 
{ 
	return CurrentScene->GetRenderSystem(); 
}

