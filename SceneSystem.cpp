#include "SceneSystem.h"

void SceneSystem::Initialize()
{
	m_currentScene.reset();
	m_sceneStack = {};
}

void SceneSystem::Update()
{
}

void SceneSystem::Finalize()
{
	m_currentScene.reset();
	m_sceneStack = {};
}
