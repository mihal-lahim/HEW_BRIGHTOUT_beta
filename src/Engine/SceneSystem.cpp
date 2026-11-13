#include "SceneSystem.h"

void SceneSystem::Initialize()
{
	if (m_currentScene)
	{
		m_currentScene->Disable();
	}

	m_currentScene.reset();
	m_sceneStack = {};
	m_pendingSceneChange = nullptr;
}

void SceneSystem::Update()
{
	// フレーム先頭で保留中のシーン遷移を実行
	ProcessPendingSceneChange();

	if (m_currentScene)
	{
		m_currentScene->UpdateScene();
	}

	// フレーム末尾で保留中のシーン遷移を実行
	ProcessPendingSceneChange();
}

void SceneSystem::Finalize()
{
	m_pendingSceneChange = nullptr;

	if (m_currentScene)
	{
		m_currentScene->Disable();
	}

	m_currentScene.reset();
	m_sceneStack = {};
}

void SceneSystem::ProcessPendingSceneChange()
{
	if (m_pendingSceneChange)
	{
		auto pendingChange = std::move(m_pendingSceneChange);
		m_pendingSceneChange = nullptr;
		pendingChange();
	}
}
