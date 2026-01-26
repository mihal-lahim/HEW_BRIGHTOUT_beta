////////////////////////////////////
//シーン管理マネージャー
//Author :hiroshi kasiwagi
//Date   :2025/12/20
////////////////////////////////////
#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Scene.h"
#include <memory>
#include <stack>

class GameObject;
class PhysicsSystem;
class ObjectManager;
class RenderSystem;


class GameManager
{
private:
	// 現在のシーンポインタ
	inline static std::unique_ptr<Scene> CurrentScene{};

	// シーンスタック
	inline static std::stack<std::unique_ptr<Scene>> SceneStack{};
public:
	// シーン変更テンプレートメソッド
	template<typename T>
		requires std::is_base_of<Scene, T>::value
	static void ChangeScene();

	// 現在のシーン取得メソッド
	static Scene* const GetCurrentScene();

	// 現在シーンをスタックに保存して新しいシーンに変更するテンプレートメソッド
	template<typename T>
		requires std::is_base_of<Scene, T>::value
	static void PushScene();

	// スタックから前のシーンを復元するテンプレートメソッド
	template<typename T>
		requires std::is_base_of<Scene, T>::value
	static void PopScene();

	// 物理演算システム取得メソッド
	static PhysicsSystem& GetPhysicsSystem();
	// オブジェクト管理システム取得メソッド
	static ObjectManager& GetObjectManager();
	// レンダリングシステム取得メソッド
	static RenderSystem& GetRenderSystem();
};


template<typename T>
	requires std::is_base_of<Scene, T>::value
inline void GameManager::ChangeScene()
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
inline void GameManager::PushScene()
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
inline void GameManager::PopScene()
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
