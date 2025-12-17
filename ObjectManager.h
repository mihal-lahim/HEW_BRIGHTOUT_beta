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


    float m_poleConnectionDistance = 30.0f;  // 電柱接続の最大距離（デフォルト: 30.0f）

public:
    ObjectManager() = default;
    ~ObjectManager() = default;

    void Initialize();
    void Finalize();
    void Update(double elapsedTime);
    void Draw() const;


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
        return ref;
    }
    
    // ゲームオブジェクトを追加するメソッド
    void AddGameObject(std::unique_ptr<GameObject> obj);

	// Destroyフラグが立っているオブジェクトを削除するメソッド
	void DestroyGameObjects();





    // 電柱を管理するメソッド
    class Pole* GetPoleByID(int poleID);
    void ConnectPolesByID(int poleID1, int poleID2);  // 指定されたID の2つの電柱を手動で接続
    
    // 電柱接続距離を設定するメソッド
    void SetPoleConnectionDistance(float distance) { m_poleConnectionDistance = distance; }
    float GetPoleConnectionDistance() const { return m_poleConnectionDistance; }

    // アイテムジェネレータオブジェクトを管理するメソッド
    class ItemGeneratorObject* GetItemGeneratorByID(int generatorID);

    // デバッグ用：すべてのAABBを描画
    void DrawDebugAABBs() const;
    void SetDebugAABBEnabled(bool enabled);
    bool IsDebugAABBEnabled() const;



	// ここから先は後方互換用の古いインターフェースです

    [[deprecated]] std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return m_GameObjects; }

    // オブジェクト作成ヘルパー関数
    // rotationY: Y軸回転角度（単位：0～360度、デフォルト0度）
    [[deprecated]] void CreateHouse(const DirectX::XMFLOAT3& position, float scale, float maxElectricity,
                     MODEL* model = nullptr, float rotationY = 0.0f, bool repaired = false);
    [[deprecated]] void CreatePole(const DirectX::XMFLOAT3& position, float height, float radius, int& poleID,
                    MODEL* model = nullptr, float rotationY = 0.0f);
    [[deprecated]] void CreateItemGenerator(const DirectX::XMFLOAT3& position, float spawnRadius, float spawnInterval,
                            int& generatorID, float rotationY = 0.0f);
    [[deprecated]] void CreateChargingSpot(const DirectX::XMFLOAT3& position, float chargeRadius, float chargeRate,
                           MODEL* model = nullptr, float rotationY = 0.0f);
    
    // Player を ObjectManager に追加して返す
    [[deprecated]] Player* CreatePlayer(int playerId, const DirectX::XMFLOAT3& pos,
                        MODEL* model, MODEL* electricModel,
                        const DirectX::XMFLOAT3& dir, class Controller* controller);
    
    // Enemy を ObjectManager に追加して返す
    [[deprecated]] Enemy* CreateEnemy(const DirectX::XMFLOAT3& position, MODEL* model, float maxHealth);

private:
    bool m_debugAABBEnabled = true;
};

#endif // OBJECT_MANAGER_H
