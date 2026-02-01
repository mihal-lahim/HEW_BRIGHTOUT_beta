#include "SceneSystem.h"

void SceneSystem::Initialize()
{
	if (m_currentScene)
	{
		m_currentScene->Disable();
	}

	m_currentScene.reset();
	m_sceneStack = {};
}

void SceneSystem::Update()
{
	m_currentScene->UpdateScene();
}

void SceneSystem::Finalize()
{
	if (m_currentScene)
	{
		m_currentScene->Disable();
	}

	m_currentScene.reset();
	m_sceneStack = {};
}
