
#ifndef GAME_OBJECT_INL
#define GAME_OBJECT_INL

#include "Scene.h"
#include "ScriptComponent.h"


template<typename T, typename... Args>
	requires std::is_base_of<Component, T>::value
T* Scene::CreateComponent(Args... args)
{
	// 新しいコンポーネントを作成
	T* newComponent = new T(args...);

	// コンポーネントプールの型IDを取得
	uint32_t typeID = Component::GetTypeID<T>();

	// 遅延構造体の作成保留リストに追加
	AddPending pending{};
	pending.ObjectPtr = newComponent;
	pending.TypeID = static_cast<uint32_t>(typeID);

	if constexpr (std::is_base_of<ScriptComponent, T>::value)
	{
		m_pendingAddScriptComponents.push(pending);
	}
	else
	{
		m_pendingAddComponents.push(pending);
	}

	return newComponent;
}

template<typename T>
	requires std::is_base_of<Component, T>::value
std::vector<T*> Scene::GetComponents() const
{
	std::vector<T*> result{};

	// コンポーネントプールの型IDを取得
	uint32_t typeID = Component::GetTypeID<T>();

	if constexpr (std::is_base_of<ScriptComponent, T>::value)
	{
		// プールが存在しない場合、空の配列を返す
		if (m_scriptComponents.size() <= typeID || m_scriptComponents.at(typeID) == nullptr)
		{
			return result;
		}
		// オブジェクトプールからスクリプトコンポーネントを取得
		auto pool = static_cast<ObjectPool<T>*>(m_scriptComponents.at(typeID).get());

		// プール内のすべてのオブジェクトを収集
		for (uint32_t i = 0;; i++)
		{
			T* comp = pool->Get(i);
			if (comp == nullptr) break;
			result.push_back(comp);
		}
		return result;
	}
	else
	{
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

	return result;
}

template<typename T>
	requires std::is_base_of<Component, T>::value
void Scene::DestroyComponent(T* component)
{
	// コンポーネントプールの型IDを取得
	uint32_t typeID = Component::GetTypeID<T>();

	// 破棄保留リストに追加
	DestroyPending pending{};
	pending.AllocationID = component->m_allocationID;
	pending.TypeID = typeID;

	if constexpr (std::is_base_of<ScriptComponent, T>::value)
	{
		m_pendingDestroyScriptComponents.push(pending);
	}
	else
	{
		m_pendingDestroyComponents.push(pending);
	}
}


#endif