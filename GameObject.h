#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <type_traits>
#include <vector>
#include "Transform.h"
#include "Object.h"


class ObjectManager;
class Component;

// ゲーム内のすべての静的オブジェクトの基底クラス
class GameObject : public Object
{
public:
	// 位置・回転・スケール情報
	Transform transform;

	GameObject();
    virtual ~GameObject() = default;

	// 所持しているコンポーネントの取得テンプレートメソッド
	template<typename T>
	 requires std::is_base_of<Component, T>::value
	T* GetComponent() const;

	// 所持しているコンポーネントの配列取得テンプレートメソッド
	template<typename T>
		requires std::is_base_of<Component, T>::value
	std::vector<T*> GetComponents() const;

	// コンポーネント追加テンプレートメソッド
	template<typename T, typename... Args>
		requires std::is_base_of<Component, T>::value
	T* AddComponent(Args... args);

	// 既存のコンポーネントを追加するメソッド
	void RegisterComponent(Component* comp);


	friend class ObjectManager;
};


#include "ObjectManager.h"

template<typename T>
	requires std::is_base_of<Component, T>::value
T* GameObject::GetComponent() const
{
	return objectManager()->template GetComponent<T>(*this);
}

template<typename T>
	requires std::is_base_of<Component, T>::value
inline std::vector<T*> GameObject::GetComponents() const
{
	return objectManager()->template GetComponents<T>(*this);
}

template<typename T, typename... Args>
	requires std::is_base_of<Component, T>::value
T* GameObject::AddComponent(Args... args)
{
	// コンポーネントを生成
	T* comp = new T(std::forward<Args>(args)...);

	// コンポーネントの所有ゲームオブジェクトを設定
	comp->m_GameObject = this;

	// オブジェクトマネージャーに登録
	objectManager()->RegisterComponent(comp);

	return comp;
}

#endif