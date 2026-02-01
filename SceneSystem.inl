
#ifndef SCENE_SYSTEM_INL
#define SCENE_SYSTEM_INL

#include "SceneSystem.h"
#include "EngineCore.h"
#include "Scene.h"

template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::ChangeScene()
{
	// 終了処理を呼び出す
	if (m_currentScene)
	{
		m_currentScene->Finalize();
		m_currentScene->Disable();
	}
	// 新しいシーンを作成して現在のシーンに設定
	m_currentScene = std::make_unique<T>();
	m_currentScene->m_gameContext = engineCore()->GetGameContext();
	// 新しいシーンの開始処理を呼び出す
	m_currentScene->Enable();
	m_currentScene->Initialize();
}


template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::PushScene()
{
	// 現在のシーンが存在する場合はスタックに保存
	if (m_currentScene)
	{
		m_currentScene->Disable();
		m_sceneStack.push(std::move(m_currentScene));
	}
	// 新しいシーンを作成して現在のシーンポインタに設定
	m_currentScene = std::make_unique<T>();
	m_currentScene->m_gameContext = engineCore()->GetGameContext();
	// 新しいシーンの開始処理を呼び出す
	m_currentScene->Enable();
	m_currentScene->Initialize();
}


template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::PopScene()
{
	// 現在のシーンが存在する場合は終了処理を呼び出す
	if (m_currentScene)
	{
		m_currentScene->Finalize();
		m_currentScene->Disable();
	}
	// スタックから前のシーンを復元
	if (!m_sceneStack.empty())
	{
		m_currentScene = std::move(m_sceneStack.top());
		m_currentScene->m_gameContext = engineCore()->GetGameContext();
		m_currentScene->Enable();
		m_sceneStack.pop();
	}
	else
	{
		// スタックが空の場合は nullptr に設定
		m_currentScene = nullptr;
	}
}




#endif
