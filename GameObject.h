#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <type_traits>
#include <vector>
#include "Component.h"
#include "Transform.h"
#include "Object.h"


class ObjectManager;

// ゲーム内のすべての静的オブジェクトの基底クラス
class GameObject : public Object
{
public:
	// 位置・回転・スケール情報
	Transform Transform;

	GameObject() = default;
    virtual ~GameObject() = default;

	// 所持しているコンポーネントの取得テンプレートメソッド
	template<ComponentDerived T>
	T* GetComponent() const;

	// 所持しているコンポーネントの配列取得テンプレートメソッド
	template<ComponentDerived T>
	std::vector<T*> GetComponents() const;

	// コンポーネント追加テンプレートメソッド
	template<ComponentDerived T, typename... Args>
	T* AddComponent(Args... args);



	friend class ObjectManager;
};

// GameObject を継承している型に制約をかけるコンセプト
template<typename T>
concept GameObjectDerived = std::is_base_of<GameObject, T>::value;



template<ComponentDerived T>
T* GameObject::GetComponent() const
{
	return ObjectManager()->GetComponent<T>();
}

template<ComponentDerived T>
inline std::vector<T*> GameObject::GetComponents() const
{
	return ObjectManager()->GetComponents<T>();
}

template<ComponentDerived T, typename... Args>
T* GameObject::AddComponent(Args... args)
{
	// コンポーネントを生成
	T* comp = new T(args...);

	// コンポーネントの所有ゲームオブジェクトを設定
	comp->m_GameObject = this;

	// オブジェクトマネージャーに登録
	ObjectManager()->RegisterComponent(comp);

	return comp;
}

#endif