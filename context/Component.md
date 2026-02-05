# `Component` クラスの使い方

## 概要
`Component` は全てのコンポーネントの基底クラスです。`GameObject` に機能を追加するための仕組みを提供し、ゲームロジック、物理、描画などの様々な機能を実装します。

---

## 基本的な関数

### 1. `GameObject& gameObject()`
この `Component` が所属している `GameObject` を取得します。

**使い方:**
```cpp
class MyComponent : public Component
{
public:
    void SomeFunction()
    {
        // 所属しているGameObjectを取得
        GameObject& obj = gameObject();
        
        // GameObjectの位置を変更
        obj.transform().SetPosition(Vector3(0.0f, 5.0f, 0.0f));
        
        // GameObjectのタグを確認
        if (obj.CompareTag("Player"))
        {
            // プレイヤーの処理
        }
    }
};
```

---

### 2. `void SetEnable(bool enable)`
`Component` の有効・無効状態を設定します。無効にすると、更新処理が呼ばれなくなります。

**使い方:**
```cpp
class MyComponent : public Component
{
public:
    void DisableTemporarily()
    {
        // コンポーネントを一時的に無効化
        SetEnable(false);
        
        // 3秒後に有効化する（仮想的な例）
        // Timer::SetTimeout(3.0f, [this]() { SetEnable(true); });
    }
};
```

---

### 3. `bool IsEnable()`
`Component` の有効・無効状態を取得します。

**使い方:**
```cpp
class MyComponent : public Component
{
public:
    void CheckState()
    {
        if (IsEnable())
        {
            // 有効な場合の処理
        }
        else
        {
            // 無効な場合の処理
        }
    }
};
```

---

### 4. `void Destroy()`
この `Component` を破棄します。破棄は即座に行われず、フレームの最後に実行されます。

**使い方:**
```cpp
class MyComponent : public Component
{
public:
    void RemoveSelf()
    {
        // このコンポーネントを破棄
        Destroy();
    }
};
```

---

## `GameObject` 生成・取得関数

### 5. `GameObject* CreateGameObject()`
新しい `GameObject` を生成します。

**使い方:**
```cpp
class BulletSpawner : public Component
{
public:
    void SpawnBullet()
    {
        // 新しい弾のGameObjectを生成
        GameObject* bullet = CreateGameObject();
        bullet->SetTag("Bullet");
        bullet->transform().SetPosition(gameObject().transform().GetPosition());
        bullet->AddComponent<BulletScript>();
    }
};
```

---

### 6. `GameObject* Instantiate(Prefab& prefab)`
`Prefab` から `GameObject` をインスタンス化します。

**使い方:**
```cpp
class EnemySpawner : public Component
{
public:
    void SpawnEnemy()
    {
        EnemyPrefab enemyPrefab;
        
        // Prefabから敵を生成
        GameObject* enemy = Instantiate(enemyPrefab);
        enemy->transform().SetPosition(Vector3(10.0f, 0.0f, 0.0f));
    }
};
```

---

### 7. `GameObject* GetGameObjectByTag(const std::string& tag)`
指定したタグを持つ最初の `GameObject` を取得します。

**使い方:**
```cpp
class AIComponent : public Component
{
public:
    void FindPlayer()
    {
        // プレイヤーオブジェクトを検索
        GameObject* player = GetGameObjectByTag("Player");
        if (player != nullptr)
        {
            Vector3 playerPos = player->transform().GetPosition();
            // プレイヤーの方向に移動する処理など
        }
    }
};
```

---

### 8. `std::vector<GameObject*> GetGameObjectsByTag(const std::string& tag)`
指定したタグを持つ全ての `GameObject` を取得します。

**使い方:**
```cpp
class ExplosionComponent : public Component
{
public:
    void DamageNearbyEnemies()
    {
        // 全ての敵オブジェクトを取得
        auto enemies = GetGameObjectsByTag("Enemy");
        
        Vector3 explosionPos = gameObject().transform().GetPosition();
        
        for (auto* enemy : enemies)
        {
            Vector3 enemyPos = enemy->transform().GetPosition();
            float distance = (enemyPos - explosionPos).Length();
            
            if (distance < 5.0f)
            {
                // 範囲内の敵にダメージを与える
            }
        }
    }
};
```

---

## `System` アクセス関数

`Component` から各種システムにアクセスできます。

### 9. システムアクセス関数
- `WindowSystem& window()`
- `PhysicsSystem& physics()`
- `SceneSystem& scene()`
- `InputSystem& input()`
- `ResourceSystem& resource()`

**使い方:**
```cpp
class PlayerController : public Component
{
public:
    void UpdateMovement()
    {
        // 入力システムにアクセス
        if (input().GetKey(KEY_W))
        {
            gameObject().transform().Translate(Vector3(0.0f, 0.0f, 1.0f));
        }
        
        // リソースシステムにアクセス
        auto* texture = resource().Load<Texture>("player.png");
        
        // 物理システムにアクセス
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (body)
        {
            physics().ApplyForce(body, Vector3(0.0f, 10.0f, 0.0f));
        }
    }
};
```

---

## ライフサイクル関数（仮想関数）

### 10. `void OnEnable()` / `void OnDisable()`
`Component` が有効化・無効化されたときに呼ばれる仮想関数です。

**使い方:**
```cpp
class MyComponent : public Component
{
public:
    void OnEnable() override
    {
        // コンポーネントが有効化されたときの処理
        std::cout << "Component enabled!" << std::endl;
    }
    
    void OnDisable() override
    {
        // コンポーネントが無効化されたときの処理
        std::cout << "Component disabled!" << std::endl;
    }
};
```

---

### 11. `void InitializeByContext()` / `void FinalizeByContext()`
`Component` が追加・削除されたときに呼ばれる仮想関数です。

**使い方:**
```cpp
class MyComponent : public Component
{
public:
    void InitializeByContext() override
    {
        // コンポーネントが追加されたときの初期化処理
        // リソースの確保など
    }
    
    void FinalizeByContext() override
    {
        // コンポーネントが削除される前のクリーンアップ処理
        // リソースの解放など
    }
};
```

---

## 静的関数

### 12. `static uint32_t GetTypeID<T>()`
指定した `Component` 型の固有IDを取得します。内部的に型の識別に使用されます。

---

## まとめ
`Component` は `GameObject` に機能を追加するための基底クラスです。`gameObject()` で所属する `GameObject` にアクセスし、`OnEnable()` / `OnDisable()` でライフサイクルを管理し、各種 `System` を使ってゲーム機能を実装します。
