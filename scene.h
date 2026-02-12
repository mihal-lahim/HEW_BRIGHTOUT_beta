/*==============================================================================

   シーン管理[scene.h]
														 Author : syota
														 Date   : 2025/12/09
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SCENE_H
#define SCENE_H

#include "Component.h"
#include "ObjectPool.h"
#include "GameContext.h"
#include <unordered_set>
#include <queue>

class GameObject;
class SceneSystem;
class Prefab;

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	// シーン有効化・無効化メソッド
	void Enable();
	void Disable();

	// シーン初期化・更新・終了メソッド
	virtual void Initialize() {};
	virtual void Finalize() {};

	// ゲームオブジェクト作成メソッド
	GameObject* CreateGameObject();

	// プレハブからゲームオブジェクトを作成するメソッド
	GameObject* Instantiate(Prefab& prefab);

	// シーン更新メソッド
	void UpdateScene();

	// シーンシステム取得メソッド
	SceneSystem& sceneSystem() const { return *m_sceneSystem; }

	// ウィンドウシステム取得メソッド
	WindowSystem& window() const;

	// 物理システム取得メソッド
	PhysicsSystem& physics() const;

	// 入力システム取得メソッド
	InputSystem& input() const;

	// レンダリングシステム取得メソッド
	RenderingSystem& rendering() const;

	// リソースシステム取得メソッド
	ResourceSystem& resource() const;

	// コンポーネント作成テンプレートメソッド
	template<typename T, typename... Args>
		requires std::is_base_of<Component, T>::value
	T* CreateComponent(Args&&... args);


	// コンポーネントの配列を受け取るテンプレートメソッド
	template<typename T>
		requires std::is_base_of<Component, T>::value
	std::vector<T*> GetComponents() const;

	// タグでゲームオブジェクトを検索するメソッド
	GameObject* GetGameObjectByTag(const std::string& tag) const;

	// タグでゲームオブジェクトを検索するメソッド
	std::vector<GameObject*> GetGameObjectsByTag(const std::string& tag) const;


	// ゲームオブジェクト破棄メソッド
	void DestroyGameObject(GameObject* gameObject);

	// コンポーネント破棄メソッド
	void DestroyComponent(Component* component);

private:
	// 現在のゲームコンテキスト
	GameContext m_gameContext{};

	// 作成保留情報構造体
	struct AddPending
	{
		Object* ObjectPtr = nullptr;
		uint32_t TypeID = 0;
	};

	// 破棄保留情報構造体
	struct DestroyPending
	{
		uint32_t AllocationID = 0;
		uint32_t TypeID = 0;
	};

	// 所属するシーンシステムポインタ
	SceneSystem* m_sceneSystem = nullptr;

	// シーン内のゲームオブジェクト配列
	std::unique_ptr<ObjectPoolBase> m_gameObjects;
	// シーン内のコンポーネント配列
	std::vector<std::unique_ptr<ObjectPoolBase>> m_components;
	// シーン内のスクリプトコンポーネント配列
	std::vector<std::unique_ptr<ObjectPoolBase>> m_scriptComponents;


	// 作成保留中のゲームオブジェクトキュー
	std::queue<GameObject*> m_pendingAddGameObjects;
	// 作成保留中のコンポーネントキュー
	std::queue<AddPending> m_pendingAddComponents;
	// 作成保留中のスクリプトコンポーネントキュー
	std::queue<AddPending> m_pendingAddScriptComponents;


	// 破棄保留中のゲームオブジェクトキュー
	std::queue<uint32_t> m_pendingDestroyGameObjects;
	// 破棄保留中のコンポーネントキュー
	std::queue<DestroyPending> m_pendingDestroyComponents;
	// 破棄保留中のスクリプトコンポーネントキュー
	std::queue<DestroyPending> m_pendingDestroyScriptComponents;


	// 登録されているスクリプトコンポーネント型IDセット
	std::unordered_set<uint32_t> m_scriptComponentTypeIDs;


	// 保留中のゲームオブジェクト追加処理メソッド
	void AddPendingGameObjectsProcess();

	// 保留中のコンポーネント追加処理メソッド
	void AddPendingComponentsProcess();

	// 保留中のスクリプトコンポーネント追加処理メソッド
	void AddPendingScriptComponentsProcess();

	// 保留中のゲームオブジェクト破棄処理メソッド
	void DestroyPendingGameObjectsProcess();

	// 保留中のコンポーネント破棄処理メソッド
	void DestroyPendingComponentsProcess();

	// 保留中のスクリプトコンポーネント破棄処理メソッド
	void DestroyPendingScriptComponentsProcess();


	// 更新処理メソッド
	void Start();
	void PreUpdate();
	void Update();
	void PostUpdate();

	// シーン更新処理メソッド
	void Cycle()
	{
		// 先に保留分を登録して InitializeByContext を実行
		AddPendingGameObjectsProcess();
		AddPendingScriptComponentsProcess();
		AddPendingComponentsProcess();

		Start();
		PreUpdate();
		Update();
		PostUpdate();

		// そのフレーム中に追加された分を処理
		AddPendingGameObjectsProcess();
		AddPendingScriptComponentsProcess();
		AddPendingComponentsProcess();

		DestroyPendingComponentsProcess();
		DestroyPendingScriptComponentsProcess();
		DestroyPendingGameObjectsProcess();
	}

	friend class SceneSystem;
};

#include "Scene.inl"

#endif
