
#ifndef SCENE_SYSTEM_INL
#define SCENE_SYSTEM_INL

#include "SceneSystem.h"

template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::ChangeScene()
{
	// 現在のシーンが存在する場合は終了処理を呼び出す
	if (CurrentScene)
	{
		CurrentScene->Finalize();
	}
	// 新しいシーンを作成して現在のシーンポインタに設定
	CurrentScene = std::make_unique<T>();
	// 新しいシーンの開始処理を呼び出す
	CurrentScene->Initialize();
}


template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::PushScene()
{
	// 現在のシーンが存在する場合はスタックに保存
	if (CurrentScene)
	{
		SceneStack.push(std::move(CurrentScene));
	}
	// 新しいシーンを作成して現在のシーンポインタに設定
	CurrentScene = std::make_unique<T>();
	// 新しいシーンの開始処理を呼び出す
	CurrentScene->Initialize();
}


template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void SceneSystem::PopScene()
{
	// 現在のシーンが存在する場合は終了処理を呼び出す
	if (CurrentScene)
	{
		CurrentScene->Finalize();
	}
	// スタックから前のシーンを復元
	if (!SceneStack.empty())
	{
		CurrentScene = std::move(SceneStack.top());
		SceneStack.pop();
	}
	else
	{
		// スタックが空の場合は nullptr に設定
		CurrentScene = nullptr;
	}
}




#endif
