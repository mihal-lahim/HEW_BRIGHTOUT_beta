#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <memory>
#include <cstdint>
#include "GameObject.h"
#include <typeindex>


// GameObject を継承している型に制約をかけるコンセプト
template<typename T>
concept GameObjectDerived = requires { std::is_base_of<GameObject, T>::value; };


class ObjectManager
{
private:
	// 更新用ゲームオブジェクトリスト
    std::vector<std::unique_ptr<GameObject>> m_GameObjects;
	// 型別ゲームオブジェクトマップ（高速検索用）
	std::unordered_map<std::type_index, std::vector<uint64_t>> m_GameObjectMap;
	// 未Startゲームオブジェクトリスト
    std::vector<GameObject*> m_PreStarted;
	// 追加保留中ゲームオブジェクトリスト
	std::vector<GameObject*> m_PendingGameObjects;

	// 更新メソッド
    void Start();
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
        Start();
        PreUpdate(elapsedTime);
        Update(elapsedTime);
        PostUpdate(elapsedTime);
        Draw();
    }


	// 型指定で単一のゲームオブジェクトを取得するテンプレートメソッド
	template<GameObjectDerived T>
    [[nodiscard]] T* GetGameObject() const
    {
        if(m_GameObjectMap.find(typeid(T)) != m_GameObjectMap.end() &&
			!m_GameObjectMap.at(typeid(T)).empty())
        {
            return static_cast<T*>(m_GameObjects.at(m_GameObjectMap.at(typeid(T)).front()));
        }
        else return nullptr;
    }

    // 型指定でゲームオブジェクトの配列を取得するテンプレートメソッド
    template<GameObjectDerived T>
    [[nodiscard]] std::vector<T*> GetGameObjects() const
    {
        if (m_GameObjectMap.find(typeid(T)) != m_GameObjectMap.end() &&
            !m_GameObjectMap.at(typeid(T)).empty())
        {
            std::vector<T*> result(m_GameObjectMap.at(typeid(T)).size());

			for (int i = 0; i < m_GameObjectMap.at(typeid(T)).size(); ++i)
            {
                result.at(i) = static_cast<T*>(m_GameObjects.at(m_GameObjectMap.at(typeid(T)).at(i)));
            }
			return result;
        }
        else return {};
    }

    // 汎用オブジェクト作成テンプレートメソッド
    // T は GameObject を継承している必要があります(コンパイルエラーになる)
    template<GameObjectDerived T, typename... Args>
    T* Create(Args... args)
    {
        T* obj = new T(args...);
        RegisterGameObject(obj);
		m_PreStarted.push_back(obj);
        return obj;
    }

    // 既存ゲームオブジェクトを登録するメソッド
    void RegisterGameObject(GameObject* obj);
};

#endif // OBJECT_MANAGER_H
