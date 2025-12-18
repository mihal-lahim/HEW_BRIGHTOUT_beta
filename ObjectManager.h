#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <memory>
#include <DirectXMath.h>
#include <cstdint>
#include "GameObject.h"
#include "debug_console.h"
#include <typeindex>


// GameObject を継承している型に制約をかけるコンセプト
template<typename T>
concept GameObjectDerived = requires { std::is_base_of<GameObject, T>::value; };

// フォワード宣言
class Player;
class Enemy;
class Controller;

class ObjectManager
{
private:
	// 更新用ゲームオブジェクトリスト
    std::vector<std::unique_ptr<GameObject>> m_GameObjects;
	// 型別ゲームオブジェクトマップ（高速検索用）
	std::unordered_map<std::type_index, std::vector<uint64_t>> m_GameObjectMap;
	// 未Startゲームオブジェクトリスト
    std::vector<GameObject*> m_PreStarted;


	// 更新メソッド
    void Start();
    void PreUpdate(double elapsedTime);
    void Update(double elapsedTime);
    void PostUpdate(double elapsedTime);
    void Draw() const;


    // ゲームオブジェクトを追加するメソッド
    void AddGameObject(std::unique_ptr<GameObject> obj);

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
    [[nodiscard]] T* Create(Args... args)
    {
        std::unique_ptr<T> obj = std::make_unique<T>(args...);
        T* ref = obj.get();
        AddGameObject(std::move(obj));
		m_PreStarted.push_back(ref);
        return ref;
    }
};

#endif // OBJECT_MANAGER_H
