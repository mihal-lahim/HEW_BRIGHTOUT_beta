# `Scene` クラスの使い方

## 概要
`Scene` は `GameObject` と `Component` の管理を行う中心的なクラスです。ゲームオブジェクトの生成・破棄、コンポーネントの取得、各種 `System` へのアクセスなど、シーン全体のライフサイクルを制御します。

---

## 基本的な関数

### 1. `GameObject* CreateGameObject()`
新しい `GameObject` を生成します。この関数は最も頻繁に使用される関数の1つです。

**使い方:**
```cpp
class MyScene : public Scene
{
public:
    void Initialize() override
    {
        // GameObjectを生成
        GameObject* player = CreateGameObject();
        player->SetTag("Player");
        
        GameObject* enemy = CreateGameObject();
        enemy->SetTag("Enemy");
    }
};
```

---

### 2. `GameObject* Instantiate(Prefab& prefab)`
`Prefab` から `GameObject` をインスタンス化します。テンプレートから複数のオブジェクトを生成する際に使用します。

**使い方:**
```cpp
class MyScene : public Scene
{
public:
    void Initialize() override
    {
        // Prefabから複数の敵を生成
        EnemyPrefab enemyPrefab;
        
        for (int i = 0; i < 10; ++i)
        {
            GameObject* enemy = Instantiate(enemyPrefab);
            enemy->transform().SetPosition(Vector3(i * 2.0f, 0.0f, 0.0f));
        }
    }
};
```

---

### 3. `void DestroyGameObject(GameObject* gameObject)`
指定した `GameObject` を破棄します。通常は `GameObject::Destroy()` を使用しますが、内部的にこの関数が呼ばれます。

---

### 4. `void DestroyComponent(Component* component)`
指定した `Component` を破棄します。通常は `Component::Destroy()` を使用しますが、内部的にこの関数が呼ばれます。

---

## `Component` 取得関数

### 5. `std::vector<T*> GetComponents<T>()`
シーン内の指定した型の全ての `Component` を取得します。

**使い方:**
```cpp
class MyScene : public Scene
{
public:
    void Update()
    {
        // シーン内の全てのPhysicsBodyコンポーネントを取得
        auto bodies = GetComponents<PhysicsBody>();
        for (auto* body : bodies)
        {
            // 各物理ボディに対する処理
        }
    }
};
```

---

### 6. `GameObject* GetGameObjectByTag(const std::string& tag)`
指定したタグを持つ最初の `GameObject` を1つ取得します。見つからない場合は `nullptr` を返します。

**使い方:**
```cpp
class MyScene : public Scene
{
public:
    void Update()
    {
        // "Player"タグを持つGameObjectを検索
        GameObject* player = GetGameObjectByTag("Player");
        if (player != nullptr)
        {
            // プレイヤーオブジェクトに対する処理
        }
    }
};
```

---

### 7. `std::vector<GameObject*> GetGameObjectsByTag(const std::string& tag)`
指定したタグを持つ全ての `GameObject` を取得します。

**使い方:**
```cpp
class MyScene : public Scene
{
public:
    void Update()
    {
        // "Enemy"タグを持つ全てのGameObjectを取得
        auto enemies = GetGameObjectsByTag("Enemy");
        for (auto* enemy : enemies)
        {
            // 各敵オブジェクトに対する処理
        }
    }
};
```

---

## `System` アクセス関数

`Scene` から各種システムにアクセスできます。

### 8. システムアクセス関数
- `SceneSystem& sceneSystem()`
- `WindowSystem& window()`
- `PhysicsSystem& physics()`
- `InputSystem& input()`
- `RenderingSystem& rendering()`
- `ResourceSystem& resource()`

**使い方:**
```cpp
class MyScene : public Scene
{
public:
    void Initialize() override
    {
        // リソースシステムを使ってモデルを読み込む
        auto* mesh = resource().Load<Mesh>("player.obj");
        
        // 物理システムにアクセス
        physics().SetGravity(Vector3(0.0f, -9.8f, 0.0f));
    }
};
```

---

## ライフサイクル関数

### 9. `void Initialize()` / `void Finalize()`
シーンの初期化と終了処理を行う仮想関数です。継承して実装します。

**使い方:**
```cpp
class MyScene : public Scene
{
public:
    void Initialize() override
    {
        // シーン開始時の初期化処理
        GameObject* player = CreateGameObject();
        player->SetTag("Player");
        player->AddComponent<PlayerController>();
    }
    
    void Finalize() override
    {
        // シーン終了時のクリーンアップ処理
    }
};
```

---

### 10. `void Enable()` / `void Disable()`
シーンの有効化・無効化を行います。シーン切り替え時に使用されます。

---

### 11. `void UpdateScene()`
シーンの更新処理を行います。通常は `SceneSystem` から自動的に呼ばれます。

---

## 内部処理の流れ

`Scene` は以下の順序で更新処理を実行します：

1. **Start()** - 新しく追加された `ScriptComponent` の `Start()` を呼び出す
2. **PreUpdate()** - 全ての `ScriptComponent` の `PreUpdate()` を呼び出す
3. **Update()** - 全ての `ScriptComponent` の `Update()` を呼び出す
4. **PostUpdate()** - 全ての `ScriptComponent` の `PostUpdate()` を呼び出す
5. **保留されたオブジェクトの追加処理**
6. **保留されたオブジェクトの破棄処理**

---

## 使用例

### 基本的なシーンの作成
```cpp
class GameScene : public Scene
{
public:
    void Initialize() override
    {
        // プレイヤーの作成
        GameObject* player = CreateGameObject();
        player->SetTag("Player");
        player->transform().SetPosition(Vector3(0.0f, 0.0f, 0.0f));
        player->AddComponent<PlayerController>();
        player->AddComponent<MeshRenderer>();
        
        // 敵の作成
        for (int i = 0; i < 5; ++i)
        {
            GameObject* enemy = CreateGameObject();
            enemy->SetTag("Enemy");
            enemy->transform().SetPosition(Vector3(i * 3.0f, 0.0f, 10.0f));
            enemy->AddComponent<EnemyAI>();
        }
        
        // ライトの作成
        GameObject* light = CreateGameObject();
        light->AddComponent<Light>();
    }
    
    void Finalize() override
    {
        // リソースの解放など
    }
};
```

### Prefabを使った複数オブジェクトの生成
```cpp
class GameScene : public Scene
{
public:
    void Initialize() override
    {
        // カスタムPrefabを使って複数の敵を配置
        EnemyPrefab enemyPrefab;
        
        for (int x = 0; x < 10; ++x)
        {
            for (int z = 0; z < 10; ++z)
            {
                GameObject* enemy = Instantiate(enemyPrefab);
                enemy->transform().SetPosition(Vector3(x * 2.0f, 0.0f, z * 2.0f));
            }
        }
    }
};
```

### タグを使ったオブジェクト検索
```cpp
class GameScene : public Scene
{
public:
    void Initialize() override
    {
        // 初期化処理
    }
    
    void SomeLogic()
    {
        // プレイヤーを検索
        GameObject* player = GetGameObjectByTag("Player");
        if (player)
        {
            Vector3 playerPos = player->transform().GetPosition();
        }
        
        // 全ての敵を検索して処理
        auto enemies = GetGameObjectsByTag("Enemy");
        for (auto* enemy : enemies)
        {
            enemy->Destroy();
        }
    }
};
```

---

## まとめ
`Scene` はゲーム世界全体を管理する重要なクラスです。`CreateGameObject()` でオブジェクトを生成し、`GetComponents()` / `GetGameObjectByTag()` で検索し、`Initialize()` / `Finalize()` でシーンのライフサイクルを管理します。全ての `GameObject` と `Component` は `Scene` によって管理され、更新されます。
