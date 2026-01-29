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

class GameContext;

class Scene
{
private:
	// 現在のゲームコンテキスト
	GameContext* m_gameContext = nullptr;
	// シーン内のゲームオブジェクト配列
	std::vector<std::vector<std::unique_ptr<ObjectPoolBase>>> m_gameObjects;
	// シーン内のコンポーネント配列
	std::vector<std::vector<std::unique_ptr<ObjectPoolBase>>> m_components;

	// 保留中のオブジェクト情報
	struct Pending
	{
		uint32_t AllocationID;
		uint32_t TypeID;
	};

	// 作成保留中のゲームオブジェクト配列
	std::vector<Pending> m_pendingCreateGameObjects;
	// 破棄保留中のゲームオブジェクト配列
	std::vector<Pending> m_pendingDestroyGameObjects;

	// 作成保留中のコンポーネント配列
	std::vector<Pending> m_pendingCreateComponents;
	// 破棄保留中のコンポーネント配列
	std::vector<Pending> m_pendingDestroyComponents;
public:

	Scene();
	virtual ~Scene();

	virtual void Initialize() {};
	void Update();
	virtual void Finalize() {};


	// ゲームオブジェクト作成テンプレートメソッド
	template<typename T, typename... Args>
		requires std::is_base_of<GameObject, T>::value
	T* CreateGameObject(Args... args);

	// コンポーネント作成テンプレートメソッド
	template<typename T, typename... Args>
		requires std::is_base_of<Component, T>::value
	T* CreateComponent(Args... args);


	// ゲームオブジェクトの配列を受け取るテンプレートメソッド
	template<typename T>
		requires std::is_base_of<GameObject, T>::value
	std::vector<T*> GetGameObjects();

	// コンポーネントの配列を受け取るテンプレートメソッド
	template<typename T>
		requires std::is_base_of<Component, T>::value
	std::vector<T*> GetComponents();
};


#endif
