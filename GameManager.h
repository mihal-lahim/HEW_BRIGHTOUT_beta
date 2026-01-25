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

// Scene を継承している型に制約をかけるコンセプト
template<typename T>
concept SceneDerived = requires { std::is_base_of<Scene, T>::value; };


// 現在のシーンポインタ
inline static std::unique_ptr<Scene> CurrentScene;

// シーンスタック
inline static std::stack<std::unique_ptr<Scene>> SceneStack;

class PhysicsSystem;


// シーン変更テンプレートメソッド
template<SceneDerived T>
void ChangeScene();

// 現在のシーン取得メソッド
Scene* GetCurrentScene() { return CurrentScene.get(); }


// 現在シーンをスタックに保存して新しいシーンに変更するテンプレートメソッド
template<SceneDerived T>
void PushScene();

// スタックから前のシーンを復元するテンプレートメソッド
template<SceneDerived T>
void PopScene();


// 物理演算システム取得メソッド
PhysicsSystem& GetPhysicsSystem() { return CurrentScene->GetPhysicsSystem(); }
// オブジェクト管理システム取得メソッド
ObjectManager& GetObjectManager() { return CurrentScene->GetObjectManager(); }
// レンダリングシステム取得メソッド
RenderSystem& GetRenderSystem() { return CurrentScene->GetRenderSystem(); }



#include "ObjectManager.h"
#include "GameObject.h"


// 汎用オブジェクト作成テンプレートメソッド
template<GameObjectDerived T, typename... Args>
T* CreateGameObject(Args... args)
{
	T* obj = new T(std::forward<Args>(args)...);
	CurrentScene->GetObjectManager().RegisterGameObject(obj);
	return obj;
}


template<SceneDerived T>
void ChangeScene()
{
	// 現在のシーンが存在する場合は終了処理を呼び出す
	if (CurrentScene)
	{
		CurrentScene->Exit();
	}
	// 新しいシーンを作成して現在のシーンポインタに設定
	CurrentScene = std::make_unique<T>();
	// 新しいシーンの開始処理を呼び出す
	CurrentScene->Enter();
}


template<SceneDerived T>
void PushScene()
{
	// 現在のシーンが存在する場合はスタックに保存
	if (CurrentScene)
	{
		SceneStack.push(std::move(CurrentScene));
	}
	// 新しいシーンを作成して現在のシーンポインタに設定
	CurrentScene = std::make_unique<T>();
	// 新しいシーンの開始処理を呼び出す
	CurrentScene->Enter();
}


template<SceneDerived T>
void PopScene()
{
	// 現在のシーンが存在する場合は終了処理を呼び出す
	if (CurrentScene)
	{
		CurrentScene->Exit();
	}
	// スタックから前のシーンを復元
	if (!SceneStack.empty())
	{
		CurrentScene = std::move(SceneStack.top());
		SceneStack.pop();
		// 復元したシーンの開始処理を呼び出す
		CurrentScene->Enter();
	}
	else
	{
		// スタックが空の場合は nullptr に設定
		CurrentScene = nullptr;
	}
}

#endif
