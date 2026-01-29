#ifndef GAME_OBJECT_INL
#define GAME_OBJECT_INL

#include "GameObject.h"
#include "GameContext.h"

template<typename T>
	requires std::is_base_of<Component, T>::value
T* GameObject::GetComponent() const
{
	// コンポーネント配列を走査して、指定された型のコンポーネントを探す
	for (const auto& comp : components)
	{
		if (comp->CompareType<T>())
		{
			return static_cast<T*>(comp.get());
		}
	}
	return nullptr; // 見つからなかった場合はnullptrを返す
}

template<typename T>
	requires std::is_base_of<Component, T>::value
std::vector<T*> GameObject::GetComponents() const
{
	std::vector<T*> result;
	// コンポーネント配列を走査して、指定された型のコンポーネントをすべて収集する
	for (const auto& comp : components)
	{
		if (comp->CompareType<T>())
		{
			result.push_back(static_cast<T*>(comp.get()));
		}
	}
	return result;
}

template<>
Transform* GameObject::GetComponent<Transform>() const
{
	return m_transform;
}

template<typename T, typename... Args>
	requires std::is_base_of<Component, T>::value
T* GameObject::AddComponent(Args... args)
{
	// TransformコンポーネントはAddComponentで追加できないようにする
	static_assert(!std::is_same_v<T, Transform>, "TransformはAddComponentで追加できません。");

	// 新しいコンポーネントを作成し、コンポーネント配列に追加する
	auto newComponent = std::make_unique<T>(args...);
	T* componentPtr = newComponent.get();
	components.push_back(std::move(newComponent));
	componentPtr->gameObject = this;
	componentPtr->gameContext = this->m_gameContext;

	return componentPtr;
}


template<typename T, typename... Args>
T* GameObject::CreateGameObject(Args... args)
{}


template<typename T>
	requires std::is_base_of<GameObject, T>::value
size_t GameObject::GetTypeID()
{
	static size_t id = counter++;
	return id;
}


#endif