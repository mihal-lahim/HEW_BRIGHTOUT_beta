#ifndef GAME_OBJECT_INL
#define GAME_OBJECT_INL

#include "GameObject.h"
#include "Scene.h"

template<typename T>
	requires std::is_base_of<Component, T>::value
T* GameObject::GetComponent() const
{
	// コンポーネント配列を走査して、指定された型のコンポーネントを探す
	for (const auto& comp : m_components)
	{
		if (comp->CompareType<T>())
		{
			return static_cast<T*>(comp);
		}
	}
	return nullptr;
}

template<typename T>
	requires std::is_base_of<Component, T>::value
std::vector<T*> GameObject::GetComponents() const
{
	std::vector<T*> result;
	// コンポーネント配列を走査して、指定された型のコンポーネントをすべて収集する
	for (const auto& comp : m_components)
	{
		if (comp->CompareType<T>())
		{
			result.push_back(static_cast<T*>(comp));
		}
	}
	return result;
}

template<>
inline Transform* GameObject::GetComponent<Transform>() const
{
	return m_transform;
}

template<typename T, typename... Args>
	requires std::is_base_of<Component, T>::value
T* GameObject::AddComponent(Args&&... args)
{
	// TransformコンポーネントはAddComponentで追加できないようにする
	static_assert(!std::is_same_v<T, Transform>, "TransformはAddComponentで追加できません。");

	// 新しいコンポーネントを作成し、コンポーネント配列に追加する
	T* componentPtr = m_scene->template CreateComponent<T>(std::forward<Args>(args)...);
	m_components.push_back(componentPtr);
	componentPtr->m_gameObject = this;

	return componentPtr;
}


#endif