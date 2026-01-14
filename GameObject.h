#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>
#include "Component.h"
#include <typeindex>
#include "Transform.h"
#include "ObjectManager.h"

// Component を継承している型に制約をかけるコンセプト
template<typename T>
concept ComponentDerived = requires { std::is_base_of<Component, T>::value; };


// ゲーム内のすべての静的オブジェクトの基底クラス
class GameObject
{
private:
    uint64_t m_ID = 0; // オブジェクトの型ごとの一意なID
    bool m_CanDestroy = false; // オブジェクトが破壊可能かどうか
	ObjectManager* m_Owner = nullptr; // 所有者の ObjectManager へのポインタ
	bool m_IsActive = true; // オブジェクトがアクティブかどうか

	// コンポーネントリスト
	std::vector<std::unique_ptr<Component>> m_Components;
	// コンポーネントマップ
	std::unordered_map<std::type_index, std::vector<uint64_t>> m_ComponentMap;
	// 未Startコンポーネントリスト
	std::vector<Component*> m_PreStarted;

	// コンポーネント破棄メソッド
	void DestroyComponents();
public:

	// 位置・回転・スケール情報
	Transform Transform;


	GameObject() = default;
    virtual ~GameObject() = default;


	// 各種更新メソッド
	void PreUpdate(double);
	void Update(double);
	void PostUpdate(double);


	// アクティブフラグの取得・設定メソッド
	bool IsActive() const { return m_IsActive; }
	void SetActive(bool isActive) { m_IsActive = isActive; }


	// Owner ObjectManager の取得メソッド
	ObjectManager* GetOwner() const { return m_Owner; }


	// 破壊可能フラグの取得・設定メソッド
	bool CanDestroy() const { return m_CanDestroy; }
	void Destroy() { m_CanDestroy = true; }


	template<ComponentDerived T>
	T* GetComponent() const;

	template<ComponentDerived T, typename... Args>
	T* AddComponent(Args... args);

	friend class ObjectManager; // ObjectManager クラスをフレンドに指定
};




template<ComponentDerived T>
T* GameObject::GetComponent() const
{
	// 型情報を取得
	std::type_index typeIndex = std::type_index(typeid(T));

	// コンポーネントマップからコンポーネントのIDを取得
	auto it = m_ComponentMap.find(typeIndex);

	if (it != m_ComponentMap.end())
	{
		uint64_t componentID = it->second;
		// コンポーネントリストからコンポーネントを取得して返す
		return static_cast<T*>(m_Components.at(componentID).get());
	}
	else
		// コンポーネントが見つからなかった場合は nullptr を返す
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