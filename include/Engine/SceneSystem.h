
#ifndef SCENE_SYSTEM_H
#define SCENE_SYSTEM_H

#include "Scene.h"
#include <type_traits>
#include <stack>
#include <memory>
#include <functional>

class EngineCore;

class SceneSystem final
{
public:
	SceneSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{ 
		Initialize();
	}
	~SceneSystem() 
	{ 
		Finalize();
	}

	// エンジンコア取得メソッド
	EngineCore* const engineCore() const 
	{ 
		return m_engineCore;
	}

	// シーンシステムの初期化
	void Initialize();
	// シーンシステムの更新
	void Update();
	// シーンシステムの終了処理
	void Finalize();


	// 現在のシーン取得メソッド
	Scene& currentScene() 
	{ 
		return *m_currentScene.get();
	}

	// シーン変更テンプレートメソッド
	template<typename T>
		requires std::is_base_of<Scene, T>::value
	void ChangeScene();

	// 現在シーンをスタックに保存して新しいシーンに変更するテンプレートメソッド
	template<typename T>
		requires std::is_base_of<Scene, T>::value
	void PushScene();

	// スタックから前のシーンを復元するテンプレートメソッド
	template<typename T>
		requires std::is_base_of<Scene, T>::value
	void PopScene();

private:
	// エンジンコアポインタ
	EngineCore* m_engineCore = nullptr;

	// 現在のシーン
	std::unique_ptr<Scene> m_currentScene{};

	// シーンスタック
	std::stack<std::unique_ptr<Scene>> m_sceneStack{};

	// 保留中のシーン遷移処理
	std::function<void()> m_pendingSceneChange{};

	// 保留中のシーン遷移を実行するメソッド
	void ProcessPendingSceneChange();

	// 即座にシーン変更を実行する内部メソッド
	template<typename T>
		requires std::is_base_of<Scene, T>::value
	void ChangeSceneImmediate();

	// 即座にシーンをプッシュする内部メソッド
	template<typename T>
		requires std::is_base_of<Scene, T>::value
	void PushSceneImmediate();

	// 即座にシーンをポップする内部メソッド
	template<typename T>
		requires std::is_base_of<Scene, T>::value
	void PopSceneImmediate();
};

#include "SceneSystem.inl"

#endif
