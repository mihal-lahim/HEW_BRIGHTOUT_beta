#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>
#include <typeinfo>
#include <memory>
#include "Component.h"
#include "Object.h"
#include "GameObject.h"


class ObjectManager
{
private:
    // GameObjectリスト
    std::vector<std::unique_ptr<GameObject>> m_GameObjects;

    // 追加保留中のGameObjectリスト
    std::vector<GameObject*> m_PendingGameObjects;

    // Componentリスト
    std::vector<std::unique_ptr<Component>> m_Components;

    // 追加保留中のComponentリスト
    std::vector<Component*> m_PendingComponents;


    // GameObjectごとのコンポーネント
	std::vector<std::vector<Component*>> m_ComponentMap;

    // 更新メソッド
	void Start();
    void PreUpdate();
    void Update();
    void PostUpdate();
    void Draw() const;

    // 保留中のゲームオブジェクトを追加するメソッド
    void AddPendingGameObjects();
    // 保留中のコンポーネントを追加するメソッド
    void AddPendingComponents();

    // Destroyフラグが立っているGameObjectを削除するメソッド
    void DestroyGameObjects();
    // Destroyフラグが立っているComponentを削除するメソッド
    void DestroyComponents();


	// ObjectID指定でGameObjectを削除するメソッド
	void DestroyGameObjectByID(ObjectID id);

	// ObjectID指定でComponentを削除するメソッド
    void DestroyComponentByID(ObjectID id);

	// ComponentMapから指定されたComponentIDを削除するメソッド
	void DestroyComponentMap(ObjectID componentID);

public:
    ObjectManager() = default;
    ~ObjectManager() = default;

    void Initialize();
    void Finalize();

    // ゲームループの1サイクルを実行するメソッド
    void Cycle()
    {
		Start();
        PreUpdate();
        Update();
        PostUpdate();
        Draw();
    }

	// 型指定で最初のゲームオブジェクトを取得するテンプレートメソッド
    template<GameObjectDerived T>
    [[nodiscard]] T* GetGameObject() const;

    // 型指定でゲームオブジェクトの配列を取得するテンプレートメソッド
    template<GameObjectDerived T>
    [[nodiscard]] std::vector<T*> GetGameObjects() const;



    template<ComponentDerived T>
    [[nodiscard]] T* GetComponent(const GameObject& obj) const;

	// 型指定でコンポーネントの配列を取得するテンプレートメソッド
    template<ComponentDerived T>
    [[nodiscard]] std::vector<T*> GetComponents(const GameObject& obj) const;


    // GameObject登録メソッド
    void RegisterGameObject(GameObject* obj);
    // Component登録メソッド
    void RegisterComponent(Component* comp);
};





template<GameObjectDerived T>
T* ObjectManager::GetGameObject() const
{
    // 型情報を取得
    std::type_info type = typeid(T);

    // 指定された型の最初のゲームオブジェクトを検索
    for (const auto& obj : m_GameObjects)
    {
        if (typeid(*obj) == type)
        {
            return static_cast<T*>(obj.get());
        }
    }
	return nullptr;
}

template<GameObjectDerived T>
std::vector<T*> ObjectManager::GetGameObjects() const
{
    // 型情報を取得
    std::type_info type = typeid(T);

    // 結果格納用配列
    std::vector<T*> result{};

    // 指定された型のゲームオブジェクトを収集
    for (const auto& obj : m_GameObjects)
    {
        if (typeid(*obj) == type)
        {
            result.push_back(static_cast<T*>(obj.get()));
        }
    }

    return result;
}

template<ComponentDerived T>
T* ObjectManager::GetComponent(const GameObject& obj) const
{
	// 取得対象のゲームオブジェクトのIDを取得
	ObjectID objID = obj.m_ID;

	// 指定された型情報を取得
	std::type_info type = typeid(T);

	for (auto* comp : m_ComponentMap.at(objID))
    {
        // 指定された型と一致するコンポーネントを返す
        if(typeid(*comp) == type)
        {
            return static_cast<T*>(comp);
        }
    }

	return nullptr;
}

template<ComponentDerived T>
std::vector<T*> ObjectManager::GetComponents(const GameObject& obj) const
{
    // 取得対象のゲームオブジェクトのIDを取得
    ObjectID objID = obj.m_ID;

    // 指定された型情報を取得
    std::type_info type = typeid(T);

    // 結果格納用配列
    std::vector<T*> result{};

    for(auto* comp : m_ComponentMap.at(objID))
    {
        // 指定された型と一致するコンポーネントを配列に追加
        if(typeid(*comp) == type)
        {
            result.push_back(static_cast<T*>(comp));
        }
	}

	return result;
}


#endif
