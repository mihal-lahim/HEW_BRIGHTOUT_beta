

#ifndef SCENE_SYSTEM_INL
#define SCENE_SYSTEM_INL

#include "SceneSystem.h"
#include "EngineCore.h"
#include "Scene.h"

// 遅延シーン変更（コンポーネントのUpdate中から呼び出し可能）
template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::ChangeScene()
{
	m_pendingSceneChange = [this]() { ChangeSceneImmediate<T>(); };
}

// 遅延シーンプッシュ
template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::PushScene()
{
	m_pendingSceneChange = [this]() { PushSceneImmediate<T>(); };
}

// 遅延シーンポップ
template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::PopScene()
{
	m_pendingSceneChange = [this]() { PopSceneImmediate<T>(); };
}

// 即座にシーン変更を実行する内部メソッド
template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::ChangeSceneImmediate()
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

// 即座にシーンをプッシュする内部メソッド
template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::PushSceneImmediate()
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

// 即座にシーンをポップする内部メソッド
template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::PopSceneImmediate()
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
