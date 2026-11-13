#ifndef SCENE_INL
#define SCENE_INL


#include "GameObject.h"
#include "Scene.h"
#include "ScriptComponent.h"


template<typename T, typename... Args>
	requires std::is_base_of<Component, T>::value
T* Scene::CreateComponent(Args&&... args)
{
	// 新しいコンポーネントを作成
	T* newComponent = new T(std::forward<Args>(args)...);

	// コンポーネントプールの型IDを取得
	uint32_t typeID = Component::GetTypeID<T>();

	// 破棄保留リストに追加
	AddPending pending{ newComponent, typeID };

	newComponent->m_typeID = typeID;

	if constexpr (std::is_base_of<ScriptComponent, T>::value)
	{
		// スクリプトコンポーネント用プールの確保
		if (m_scriptComponents.size() <= typeID)
		{
			m_scriptComponents.resize(typeID + 1);
		}
		if (!m_scriptComponents.at(typeID))
		{
			m_scriptComponents.at(typeID) = std::make_unique<ObjectPool<T>>();
		}

		m_scriptComponentTypeIDs.insert(typeID);
		m_pendingAddScriptComponents.push(pending);
	}
	else
	{
		// 通常コンポーネント用プールの確保
		if (m_components.size() <= typeID)
		{
			m_components.resize(typeID + 1);
		}
		if (!m_components.at(typeID))
		{
			m_components.at(typeID) = std::make_unique<ObjectPool<T>>();
		}

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
		for (size_t i = 0; i < pool->Size(); ++i)
		{
			auto* comp = pool->Get(static_cast<uint32_t>(i));
			if (comp)
			{
				result.push_back(static_cast<T*>(comp));
			}
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
		for (size_t i = 0; i < pool->Size(); ++i)
		{
			auto* comp = pool->Get(static_cast<uint32_t>(i));
			if (comp)
			{
				result.push_back(static_cast<T*>(comp));
			}
		}
		return result;
	}

	return result;
}


#endif