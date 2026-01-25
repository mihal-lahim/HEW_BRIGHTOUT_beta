#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <type_traits>
#include "Component.h"
#include "Transform.h"
#include "Object.h"


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

	// コンポーネント追加テンプレートメソッド
	template<ComponentDerived T, typename... Args>
	T* AddComponent(Args... args);


	friend class ObjectManager;
};


#include "ObjectManager.h"

// GameObject を継承している型に制約をかけるコンセプト
template<typename T>
concept GameObjectDerived = std::is_base_of<GameObject, T>::value;


template<ComponentDerived T>
T* GameObject::GetComponent() const
{
	// 型情報を取得
	std::type_index typeIndex = std::type_index(typeid(T));

	for (auto& comp : m_Components)
	{
		// 指定された型と一致するコンポーネントを返す
		if(typeid(*comp) == typeIndex)
		{
			return static_cast<T*>(comp);
		}
	}

	// 見つからなかった場合は nullptr を返す
	return nullptr;
}


template<ComponentDerived T, typename... Args>
T* GameObject::AddComponent(Args... args)
{
	// 新しいコンポーネントを作成
	std::unique_ptr<T> newComponent = std::make_unique<T>(args...);
	// コンポーネントの型情報を取得
	std::type_index typeIndex = std::type_index(typeid(T));
	// コンポーネントIDを決定
	uint64_t componentID = m_Components.size();

	// コンポーネントマップに登録
	m_ComponentMap[typeIndex].push_back(componentID);
	// コンポーネントの所有者を設定
	newComponent->m_Owner = this;
	// コンポーネントIDを設定
	newComponent->m_ID = componentID;
	// コンポーネントリストに追加
	m_Components.push_back(std::move(newComponent));
	// コンポーネントのStartを保留リストに追加
	m_PreStarted.push_back(m_Components.back().get());

	// オーナーのObjectManagerにコンポーネントを登録
	m_Owner->RegisterComponent(m_Components.back().get());

	// 追加したコンポーネントを返す
	return static_cast<T*>(m_Components.back().get());
}

#endif