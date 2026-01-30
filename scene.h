/*==============================================================================

   シーン管理[scene.h]
														 Author : syota
														 Date   : 2025/12/09
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SCENE_H
#define SCENE_H

#include "GameObject.h"
#include "Component.h"
#include "ObjectPool.h"
#include <queue>


struct GameContext;
class SceneSystem;

class Scene
{
private:
	// 所属するシーンシステムポインタ
	SceneSystem* m_sceneSystem = nullptr;

	// シーン内のゲームオブジェクト配列
	std::unique_ptr<ObjectPoolBase> m_gameObjects;
	// シーン内のコンポーネント配列
	std::vector<std::unique_ptr<ObjectPoolBase>> m_components;
	// シーン内のスクリプトコンポーネント配列
	std::vector<std::unique_ptr<ObjectPoolBase>> m_scriptComponents;

	// 作成保留情報構造体
	struct AddPending
	{
		Object* ObjectPtr = nullptr;
		uint32_t TypeID = 0;
	};

	// 作成保留中のゲームオブジェクトキュー
	std::queue<GameObject*> m_pendingAddGameObjects;
	// 作成保留中のコンポーネントキュー
	std::queue<AddPending> m_pendingAddComponents;
	// 作成保留中のスクリプトコンポーネントキュー
	std::queue<AddPending> m_pendingAddScriptComponents;

	// 破棄保留情報構造体
	struct DestroyPending
	{
		uint32_t AllocationID = 0;
		uint32_t TypeID = 0;
	};

	// 破棄保留中のゲームオブジェクトキュー
	std::queue<uint32_t> m_pendingDestroyGameObjects;
	// 破棄保留中のコンポーネントキュー
	std::queue<DestroyPending> m_pendingDestroyComponents;
	// 破棄保留中のスクリプトコンポーネントキュー
	std::queue<DestroyPending> m_pendingDestroyScriptComponents;

	// スタート前情報構造体
	struct PreStart
	{
		uint32_t AllocationID = 0;
		uint32_t TypeID = 0;
	};


	// スタート前のスクリプトコンポーネントキュー
	std::queue<PreStart> m_preStartScriptComponents;

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
		DestroyPendingComponentsProcess();
		DestroyPendingScriptComponentsProcess();
		DestroyPendingGameObjectsProcess();

		AddPendingGameObjectsProcess();
		AddPendingScriptComponentsProcess();
		AddPendingComponentsProcess();

		Start();
		PreUpdate();
		Update();
		PostUpdate();
	}
public:

	Scene() = default;
	virtual ~Scene() = default;

	// シーン初期化・更新・終了メソッド
	virtual void Initialize() {};
	void UpdateScene();
	virtual void Finalize() {};


	// シーンシステム取得メソッド
	SceneSystem& sceneSystem() const { return *m_sceneSystem; }

	// ゲームオブジェクトメソッド
	GameObject* CreateGameObject();

	// コンポーネント作成テンプレートメソッド
	template<typename T, typename... Args>
		requires std::is_base_of<Component, T>::value
	T* CreateComponent(Args... args);



	// コンポーネントの配列を受け取るテンプレートメソッド
	template<typename T>
		requires std::is_base_of<Component, T>::value
	std::vector<T*> GetComponents() const;

	// ゲームオブジェクト破棄メソッド
	void DestroyGameObject(GameObject* gameObject);

	// コンポーネント破棄メソッド
	template<typename T>
		requires std::is_base_of<Component, T>::value
	void DestroyComponent(T* component);
};


#endif
