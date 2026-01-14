#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <memory>
#include <cstdint>
#include <typeindex>
#include "PhysicsSystem.h"

class GameObject;

// GameObject を継承している型に制約をかけるコンセプト
template<typename T>
concept GameObjectDerived = requires { std::is_base_of<GameObject, T>::value; };


class ObjectManager
{
private:
	// 更新用ゲームオブジェクトリスト
    std::vector<std::unique_ptr<GameObject>> m_GameObjects{};
	// 型別ゲームオブジェクトマップ（高速検索用）
    std::unordered_map<std::type_index, std::vector<uint64_t>> m_GameObjectMap{};
	// 追加保留中ゲームオブジェクトリスト
    std::vector<GameObject*> m_PendingGameObjects{};

	// コンポーネントの型別マップ
    std::unordered_map<std::type_index, std::vector<Component*>> m_ComponentMap{};

	// 物理演算システム
    PhysicsSystem m_PhysicsSystem{};


	// 更新メソッド
	void PhysicsUpdate(double elapsedTime);
    void PreUpdate(double elapsedTime);
    void Update(double elapsedTime);
    void PostUpdate(double elapsedTime);
    void Draw() const;

	// 保留中のゲームオブジェクトを追加するメソッド
	void AddPendingGameObjects();

    // Destroyフラグが立っているオブジェクトを削除するメソッド
    void DestroyGameObjects();

public:
    ObjectManager() = default;
    ~ObjectManager() = default;

    void Initialize();
    void Finalize();

	// ゲームループの1サイクルを実行するメソッド
    void Cycle(double elapsedTime)
    {
		PhysicsUpdate(elapsedTime);
        PreUpdate(elapsedTime);
        Update(elapsedTime);
        PostUpdate(elapsedTime);
        Draw();
    }


	// 型指定で単一のゲームオブジェクトを取得するテンプレートメソッド
	template<GameObjectDerived T>
    [[nodiscard]] T* GetGameObject() const
    {
        if(!m_GameObjectMap.at(typeid(T)).empty() &&
            m_GameObjectMap.find(typeid(T)) != m_GameObjectMap.end())
        {
            return static_cast<T*>(m_GameObjects.at(m_GameObjectMap.at(typeid(T)).front()));
        }
        else return nullptr;
    }

    // 型指定でゲームオブジェクトの配列を取得するテンプレートメソッド
    template<GameObjectDerived T>
    [[nodiscard]] std::vector<T*> GetGameObjects() const
    {
        if (!m_GameObjectMap.at(typeid(T)).empty() &&
             m_GameObjectMap.find(typeid(T)) != m_GameObjectMap.end())
        {
            std::vector<T*> result(m_GameObjectMap.at(typeid(T)).size());

			for (int i = 0; i < m_GameObjectMap.at(typeid(T)).size(); ++i)
            {
                result.at(i) = dynamic_cast<T*>(m_GameObjects.at(m_GameObjectMap.at(typeid(T)).at(i)));
            }
			return result;
        }
        else return {};
    }


	// 型指定でコンポーネントの配列を取得するテンプレートメソッド
    template<ComponentDerived T>
    [[nodiscard]] std::vector<T*> GetComponents() const
    {
        if (!m_ComponentMap.at(typeid(T)).empty() &&
             m_ComponentMap.find(typeid(T)) != m_ComponentMap.end())
        {
			std::vector<T*> result(m_ComponentMap.at(typeid(T)).size());

			for (int i = 0; i < m_ComponentMap.at(typeid(T)).size(); ++i)
            {
                result.at(i) = dynamic_cast<T*>(m_ComponentMap.at(typeid(T)).at(i));
            }
			return result;
        }
    }

    // 汎用オブジェクト作成テンプレートメソッド
    // T は GameObject を継承している必要があります(コンパイルエラーになる)
    template<GameObjectDerived T, typename... Args>
    T* Create(Args... args)
    {
        T* obj = new T(args...);
        RegisterGameObject(static_cast<GameObject*>(obj));
        return obj;
    }

    // 既存ゲームオブジェクトを登録するメソッド
    void RegisterGameObject(GameObject* obj);

	// コンポーネントを登録するメソッド
	void RegisterComponent(Component* component);
};

#endif
