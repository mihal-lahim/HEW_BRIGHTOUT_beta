# System 類の概要

## 概要
このプロジェクトでは、ゲームの各機能を管理する複数の `System` クラスが存在します。それぞれの `System` は特定の役割を持ち、`GameObject` や `Component` から簡単にアクセスできます。

---

## System の一覧

### 1. `SceneSystem`
**役割:** シーンの管理とシーン切り替えを行います。

**主な機能:**
- 現在のシーンの取得
- シーンの切り替え
- シーンのスタック管理（Push/Pop）

**アクセス方法:**
```cpp
// GameObjectから
SceneSystem& sceneSystem = gameObject().scene();

// Componentから
SceneSystem& sceneSystem = scene();

// Sceneから
SceneSystem& sceneSystem = sceneSystem();
```

**使い方:**
```cpp
class GameClearTrigger : public ScriptComponent
{
public:
    void OnTriggerEnter(GameObject* other)
    {
        if (other->CompareTag("Player"))
        {
            // クリアシーンに切り替え
            scene().ChangeScene<ClearScene>();
        }
    }
};
```

**主要メソッド:**
- `Scene& currentScene()` - 現在のシーンを取得
- `void ChangeScene<T>()` - シーンを切り替える
- `void PushScene<T>()` - 現在のシーンをスタックに保存して新しいシーンに切り替え
- `void PopScene<T>()` - スタックから前のシーンを復元

---

### 2. `WindowSystem`
**役割:** ウィンドウの管理とウィンドウメッセージの処理を行います。

**主な機能:**
- ウィンドウの作成・管理
- ウィンドウメッセージのディスパッチ
- メッセージハンドラーの登録

**アクセス方法:**
```cpp
// GameObjectから
WindowSystem& windowSystem = gameObject().window();

// Componentから
WindowSystem& windowSystem = window();

// Sceneから
WindowSystem& windowSystem = window();
```

**使い方:**
```cpp
class WindowInfo : public ScriptComponent
{
public:
    void Start() override
    {
        WindowSystem& winSys = window();
        
        // ウィンドウ情報の取得など
    }
};
```

---

### 3. `PhysicsSystem`
**役割:** 物理演算と衝突判定を管理します。Bullet Physics エンジンを使用しています。

**主な機能:**
- 物理ボディの登録・登録解除
- 物理演算の更新
- 衝突判定（Collision）
- トリガー判定（Trigger）
- レイキャスト

**アクセス方法:**
```cpp
// GameObjectから
PhysicsSystem& physicsSystem = gameObject().physics();

// Componentから
PhysicsSystem& physicsSystem = physics();

// Sceneから
PhysicsSystem& physicsSystem = physics();
```

**使い方:**
```cpp
class Bullet : public ScriptComponent
{
public:
    void Start() override
    {
        // PhysicsBodyを取得
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (body)
        {
            // 物理システムに登録
            physics().RegisterPhysicsBody(body);
        }
    }
    
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (body)
        {
            // 衝突判定を取得
            auto collisions = physics().GetCollisionEnter(body);
            for (auto* other : collisions)
            {
                std::cout << "Hit!" << std::endl;
                gameObject().Destroy();
            }
        }
    }
};
```

**主要メソッド:**
- `void RegisterPhysicsBody(PhysicsBody* body)` - 物理ボディを登録
- `void UnregisterPhysicsBody(PhysicsBody* body)` - 物理ボディの登録解除
- `std::vector<PhysicsBody*> GetCollisionEnter(PhysicsBody* obj)` - 衝突開始を取得
- `std::vector<PhysicsBody*> GetCollisionStay(PhysicsBody* obj)` - 衝突継続を取得
- `std::vector<PhysicsBody*> GetCollisionExit(PhysicsBody* obj)` - 衝突終了を取得
- `std::vector<PhysicsBody*> GetTriggerEnter(PhysicsBody* obj)` - トリガー開始を取得
- `std::vector<PhysicsBody*> GetTriggerStay(PhysicsBody* obj)` - トリガー継続を取得
- `std::vector<PhysicsBody*> GetTriggerExit(PhysicsBody* obj)` - トリガー終了を取得

---

### 4. `InputSystem`
**役割:** キーボードとゲームパッドの入力を管理します。

**主な機能:**
- キーボード入力の取得
- ゲームパッド入力の取得（最大4台）
- 入力状態の更新

**アクセス方法:**
```cpp
// GameObjectから
InputSystem& inputSystem = gameObject().input();

// Componentから
InputSystem& inputSystem = input();

// Sceneから
InputSystem& inputSystem = input();
```

**使い方:**
```cpp
class PlayerController : public ScriptComponent
{
public:
    void Update() override
    {
        // キーボード入力
        Keyboard& keyboard = input().keyboard();
        
        if (keyboard.GetKey(VK_W))
        {
            // 前進
        }
        
        if (keyboard.GetKeyDown(VK_SPACE))
        {
            // ジャンプ
        }
        
        // ゲームパッド入力
        GamePad& gamePad = input().gamePad(0); // プレイヤー1
        
        if (gamePad.GetButton(XINPUT_GAMEPAD_A))
        {
            // Aボタンが押された
        }
    }
};
```

**主要メソッド:**
- `Keyboard& keyboard()` - キーボードデバイスを取得
- `GamePad& gamePad(int index)` - ゲームパッドデバイスを取得（0～3）

---

### 5. `RenderingSystem`
**役割:** 描画処理を管理します。

**主な機能:**
- グラフィックスデバイスの管理
- シーンの描画
- レンダリングパイプラインの制御

**アクセス方法:**
```cpp
// GameObjectから
RenderingSystem& renderingSystem = gameObject().rendering();

// Componentから
RenderingSystem& renderingSystem = rendering();

// Sceneから
RenderingSystem& renderingSystem = rendering();
```

**使い方:**
```cpp
class CustomRenderer : public ScriptComponent
{
public:
    void Update() override
    {
        // グラフィックスデバイスへのアクセス
        GraphicsDevice& gfxDevice = rendering().GetGraphicsDevice();
        
        // カスタム描画処理など
    }
};
```

**主要メソッド:**
- `GraphicsDevice& GetGraphicsDevice()` - グラフィックスデバイスを取得
- `void Render(const Scene& scene)` - シーンを描画

---

### 6. `ResourceSystem`
**役割:** リソース（メッシュ、テクスチャ、シェーダーなど）の読み込みと管理を行います。

**主な機能:**
- リソースの読み込み
- リソースのキャッシュ管理
- リソースの解放

**アクセス方法:**
```cpp
// GameObjectから
ResourceSystem& resourceSystem = gameObject().resource();

// Componentから
ResourceSystem& resourceSystem = resource();

// Sceneから
ResourceSystem& resourceSystem = resource();
```

**使い方:**
```cpp
class ModelLoader : public ScriptComponent
{
public:
    void Start() override
    {
        // メッシュを読み込む
        auto* mesh = resource().Load<Mesh>("models/player.obj");
        
        // テクスチャを読み込む
        auto* texture = resource().Load<Texture>("textures/player.png");
        
        // シェーダーを読み込む
        auto* shader = resource().Load<Shader>("shaders/default.hlsl");
        
        // MeshRendererに設定
        auto* renderer = gameObject().GetComponent<MeshRenderer>();
        if (renderer)
        {
            renderer->SetMesh(mesh);
            renderer->SetTexture(texture);
        }
    }
};
```

**主要メソッド:**
- `T* Load<T>(const std::string& path)` - リソースを読み込む

---

## System へのアクセス方法まとめ

### GameObject からアクセス
```cpp
GameObject* obj = scene.CreateGameObject();

SceneSystem& sceneSystem = obj->scene();
WindowSystem& windowSystem = obj->window();
PhysicsSystem& physicsSystem = obj->physics();
InputSystem& inputSystem = obj->input();
RenderingSystem& renderingSystem = obj->rendering();
ResourceSystem& resourceSystem = obj->resource();
```

### Component からアクセス
```cpp
class MyComponent : public Component
{
public:
    void SomeFunction()
    {
        SceneSystem& sceneSystem = scene();
        WindowSystem& windowSystem = window();
        PhysicsSystem& physicsSystem = physics();
        InputSystem& inputSystem = input();
        ResourceSystem& resourceSystem = resource();
    }
};
```

### Scene からアクセス
```cpp
class MyScene : public Scene
{
public:
    void Initialize() override
    {
        SceneSystem& sceneSystem = sceneSystem();
        WindowSystem& windowSystem = window();
        PhysicsSystem& physicsSystem = physics();
        InputSystem& inputSystem = input();
        RenderingSystem& renderingSystem = rendering();
        ResourceSystem& resourceSystem = resource();
    }
};
```

---

## 実用例

### シーン切り替え
```cpp
class GameClearZone : public ScriptComponent
{
public:
    void OnTriggerEnter(GameObject* other)
    {
        if (other->CompareTag("Player"))
        {
            // クリアシーンに切り替え
            scene().ChangeScene<ClearScene>();
        }
    }
};
```

---

### 物理演算と衝突判定
```cpp
class Bullet : public ScriptComponent
{
public:
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (!body) return;
        
        // 衝突したオブジェクトを取得
        auto collisions = physics().GetCollisionEnter(body);
        for (auto* collision : collisions)
        {
            GameObject& other = collision->gameObject();
            
            if (other.CompareTag("Enemy"))
            {
                // 敵にダメージを与える
                auto* health = other.GetComponent<HealthComponent>();
                if (health)
                {
                    health->TakeDamage(10.0f);
                }
                
                // 弾丸を破棄
                gameObject().Destroy();
            }
        }
    }
};
```

---

### 入力処理
```cpp
class PlayerInput : public ScriptComponent
{
public:
    void Update() override
    {
        Keyboard& kb = input().keyboard();
        
        // キーボード入力
        if (kb.GetKey(VK_W)) { /* 前進 */ }
        if (kb.GetKey(VK_S)) { /* 後退 */ }
        if (kb.GetKey(VK_A)) { /* 左移動 */ }
        if (kb.GetKey(VK_D)) { /* 右移動 */ }
        if (kb.GetKeyDown(VK_SPACE)) { /* ジャンプ */ }
        
        // ゲームパッド入力
        GamePad& pad = input().gamePad(0);
        if (pad.GetButton(XINPUT_GAMEPAD_A)) { /* ジャンプ */ }
    }
};
```

---

### リソース読み込み
```cpp
class AssetLoader : public ScriptComponent
{
public:
    void Start() override
    {
        // プレイヤーモデルを読み込む
        auto* playerMesh = resource().Load<Mesh>("models/player.obj");
        auto* playerTexture = resource().Load<Texture>("textures/player.png");
        
        // 敵モデルを読み込む
        auto* enemyMesh = resource().Load<Mesh>("models/enemy.obj");
        
        // シェーダーを読み込む
        auto* shader = resource().Load<Shader>("shaders/default.hlsl");
    }
};
```

---

## まとめ
各 `System` は特定の機能を管理し、`GameObject` / `Component` / `Scene` から簡単にアクセスできます。`SceneSystem` でシーン管理、`PhysicsSystem` で物理演算、`InputSystem` で入力処理、`RenderingSystem` で描画、`ResourceSystem` でリソース管理を行います。これらを組み合わせることで、ゲームの様々な機能を実装できます。
