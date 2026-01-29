
#ifndef GAME_OBJECT_INL
#define GAME_OBJECT_INL

#include "Scene.h"

template<typename T, typename... Args>
	requires std::is_base_of<GameObject, T>::value
T* Scene::CreateGameObject(Args... args)
{
	// ゲームオブジェクトプールの型IDを取得
	size_t typeID = GameObject::GetTypeID<T>();

	// プールが存在しない場合、新規作成
	if (m_gameObjects.size() <= typeID)
	{
		m_gameObjects.resize(typeID + 1);
	}
	if (m_gameObjects.at(typeID) == nullptr)
	{
		m_gameObjects.at(typeID) = std::make_unique<ObjectPool<T>>();
	}

	// オブジェクトプールから新しいゲームオブジェクトを作成
	auto pool = static_cast<ObjectPool<T>*>(m_gameObjects.at(typeID).get());
	T* newGameObject = pool->Create(args...);

	// ゲームコンテキストを設定
	newGameObject->m_gameContext = m_gameContext;

	// 作成保留リストに追加
	Pending pending{};
	pending.AllocationID = newGameObject->m_allocationID;
	pending.TypeID = static_cast<uint32_t>(typeID);
	m_pendingCreateGameObjects.push_back(pending);
	return newGameObject;
}

template<typename T, typename... Args>
	requires std::is_base_of<Component, T>::value
T* Scene::CreateComponent(Args... args)
{
	// コンポーネントプールの型IDを取得
	size_t typeID = Component::GetTypeID<T>();

	// プールが存在しない場合、新規作成
	if (m_components.size() <= typeID)
	{
		m_components.resize(typeID + 1);
	}
	if (m_components.at(typeID) == nullptr)
	{
		m_components.at(typeID) = std::make_unique<ObjectPool<T>>();
	}

	// オブジェクトプールから新しいコンポーネントを作成
	auto pool = static_cast<ObjectPool<T>*>(m_components.at(typeID).get());
	T* newComponent = pool->Create(args...);

	// ゲームコンテキストを設定
	newComponent->m_gameContext = m_gameContext;

	// 作成保留リストに追加
	Pending pending{};
	pending.AllocationID = newComponent->m_allocationID;
	pending.TypeID = static_cast<uint32_t>(typeID);
	m_pendingCreateComponents.push_back(pending);
	return newComponent;
}


template<typename T>
	requires std::is_base_of<GameObject, T>::value
std::vector<T*> Scene::GetGameObjects()
{
	std::vector<T*> result;

	// ゲームオブジェクトプールの型IDを取得
	size_t typeID = GameObject::GetTypeID<T>();

	// プールが存在しない場合、空の配列を返す
	if (m_gameObjects.size() <= typeID || m_gameObjects.at(typeID) == nullptr)
	{
		return result;
	}

	// オブジェクトプールからゲームオブジェクトを取得
	auto pool = static_cast<ObjectPool<T>*>(m_gameObjects.at(typeID).get());

	// プール内のすべてのオブジェクトを収集
	for (uint32_t i = 0;; i++)
	{
		T* obj = pool->Get(i);
		if (obj == nullptr) break;
		result.push_back(obj);
	}
	return result;
}

template<typename T>
	requires std::is_base_of<Component, T>::value
std::vector<T*> Scene::GetComponents()
{
	std::vector<T*> result;

	// コンポーネントプールの型IDを取得
	size_t typeID = Component::GetTypeID<T>();

	// プールが存在しない場合、空の配列を返す
	if (m_components.size() <= typeID || m_components.at(typeID) == nullptr)
	{
		return result;
	}

	// オブジェクトプールからコンポーネントを取得
	auto pool = static_cast<ObjectPool<T>*>(m_components.at(typeID).get());

	// プール内のすべてのオブジェクトを収集
	for (uint32_t i = 0;; i++)
	{
		T* comp = pool->Get(i);
		if (comp == nullptr) break;
		result.push_back(comp);
	}
	return result;
}


#endif