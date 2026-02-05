# `PhysicsBody` と `ColliderShape` の使い方

## 概要
`PhysicsBody` は物理演算を適用するためのコンポーネントで、`ColliderShape` は衝突判定の形状を定義するコンポーネントです。これらを組み合わせることで、オブジェクトに物理的な振る舞いと衝突判定を持たせることができます。

---

# `PhysicsBody` クラス

## BodyType（ボディタイプ）

物理ボディには3つのタイプがあります：

### 1. `STATIC`（静的）
- **用途**: 動かない地形、壁、床など
- **特徴**: 
  - 物理演算の影響を受けない
  - 他のオブジェクトとの衝突判定は行う
  - 質量は無視される

### 2. `DYNAMIC`（動的）
- **用途**: プレイヤー、敵、弾丸など動くオブジェクト
- **特徴**: 
  - 重力や力の影響を受ける
  - 他のオブジェクトとの衝突で反応する
  - 質量が必要

### 3. `KINEMATIC`（キネマティック）
- **用途**: スクリプトで制御する動くオブジェクト（エレベーター、動く床など）
- **特徴**: 
  - 物理演算の影響を受けない
  - スクリプトで直接位置を変更できる
  - 他のオブジェクトに衝突を与える

---

## PhysicsBodyDesc（物理ボディの設定）

`PhysicsBody` を作成する際は、`PhysicsBodyDesc` 構造体で設定を行います。

```cpp
struct PhysicsBodyDesc
{
    BodyType Type = BodyType::STATIC;      // ボディタイプ
    bool IsTrigger = false;                 // トリガーかどうか
    float Mass = 1.0f;                      // 質量（DYNAMICの場合のみ有効）
    Vector3 Gravity = {0.0f, -9.81f, 0.0f}; // 重力（DYNAMICの場合のみ有効）
    Vector3 FixedRotation = {0.0f, 0.0f, 0.0f}; // 固定する回転軸（1で固定、0で自由）
};
```

---

## 基本的な使い方

### 物理ボディの追加
```cpp
GameObject* obj = scene.CreateGameObject();

// PhysicsBodyDescで設定を作成
PhysicsBodyDesc desc;
desc.Type = BodyType::DYNAMIC;
desc.Mass = 10.0f;
desc.Gravity = Vector3(0.0f, -9.81f, 0.0f);
desc.IsTrigger = false;

// PhysicsBodyコンポーネントを追加
auto* body = obj->AddComponent<PhysicsBody>(desc);

// ColliderShapeも必ず追加（形状の定義）
BoxColliderDesc boxDesc;
boxDesc.SizeX = 1.0f;
boxDesc.SizeY = 1.0f;
boxDesc.SizeZ = 1.0f;
obj->AddComponent<ColliderShape>(boxDesc);
```

---

## 力とインパルスの適用

### `void ApplyForce(const Vector3& force)`
オブジェクトに力を加えます。継続的に力を加える場合に使用します。

**使い方:**
```cpp
class PlayerController : public ScriptComponent
{
public:
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (body)
        {
            // 前方向に力を加える
            Vector3 force = Vector3(0.0f, 0.0f, 100.0f) * Time::DeltaTime();
            body->ApplyForce(force);
        }
    }
};
```

---

### `void ApplyImpulse(const Vector3& impulse)`
オブジェクトに瞬間的な力（インパルス）を加えます。ジャンプや爆発などに使用します。

**使い方:**
```cpp
class Jump : public ScriptComponent
{
public:
    void Update() override
    {
        if (input().keyboard().GetKeyDown(VK_SPACE))
        {
            auto* body = gameObject().GetComponent<PhysicsBody>();
            if (body)
            {
                // 上方向にジャンプ
                Vector3 jumpImpulse = Vector3(0.0f, 10.0f, 0.0f);
                body->ApplyImpulse(jumpImpulse);
            }
        }
    }
};
```

---

## 速度の設定と取得

### `void SetVelocity(const Vector3& velocity)`
オブジェクトの速度を直接設定します。

**使い方:**
```cpp
class BulletScript : public ScriptComponent
{
public:
    void Start() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (body)
        {
            // 弾丸を前方に発射
            Vector3 velocity = Vector3(0.0f, 0.0f, 20.0f);
            body->SetVelocity(velocity);
        }
    }
};
```

---

### `Vector3 GetVelocity()`
オブジェクトの現在の速度を取得します。

**使い方:**
```cpp
class SpeedMonitor : public ScriptComponent
{
public:
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (body)
        {
            Vector3 velocity = body->GetVelocity();
            float speed = velocity.Length();
            
            std::cout << "Current speed: " << speed << std::endl;
        }
    }
};
```

---

## 衝突判定の取得

### Collision（通常の衝突）
物理的に押し出しが発生する衝突です。

#### `std::vector<PhysicsBody*> GetCollisionEnter()`
衝突が開始されたフレームに呼ばれます。

#### `std::vector<PhysicsBody*> GetCollisionStay()`
衝突が継続している間、毎フレーム呼ばれます。

#### `std::vector<PhysicsBody*> GetCollisionExit()`
衝突が終了したフレームに呼ばれます。

**使い方:**
```cpp
class BulletCollision : public ScriptComponent
{
public:
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (!body) return;
        
        // 衝突開始を検出
        auto collisions = body->GetCollisionEnter();
        for (auto* other : collisions)
        {
            GameObject& otherObj = other->gameObject();
            
            if (otherObj.CompareTag("Enemy"))
            {
                std::cout << "Hit enemy!" << std::endl;
                
                // 敵にダメージを与える
                auto* health = otherObj.GetComponent<HealthComponent>();
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

## トリガー判定の取得

### Trigger（トリガー）
物理的な押し出しが発生しない衝突判定です。検出だけを行います。

#### `std::vector<PhysicsBody*> GetTriggerEnter()`
トリガーに入ったフレームに呼ばれます。

#### `std::vector<PhysicsBody*> GetTriggerStay()`
トリガー内にいる間、毎フレーム呼ばれます。

#### `std::vector<PhysicsBody*> GetTriggerExit()`
トリガーから出たフレームに呼ばれます。

**使い方:**
```cpp
class ClearZone : public ScriptComponent
{
public:
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (!body) return;
        
        // トリガーに入ったオブジェクトを検出
        auto triggers = body->GetTriggerEnter();
        for (auto* other : triggers)
        {
            GameObject& otherObj = other->gameObject();
            
            if (otherObj.CompareTag("Player"))
            {
                std::cout << "Player cleared the game!" << std::endl;
                
                // クリアシーンに切り替え
                scene().ChangeScene<ClearScene>();
            }
        }
    }
};
```

---

# `ColliderShape` クラス

## 概要
`ColliderShape` は衝突判定の形状を定義します。`PhysicsBody` と一緒に使用する必要があります。

---

## コライダーの種類

### 1. `BoxCollider`（ボックス）
直方体の形状です。

**使い方:**
```cpp
GameObject* obj = scene.CreateGameObject();

// PhysicsBodyを追加
PhysicsBodyDesc bodyDesc;
bodyDesc.Type = BodyType::STATIC;
obj->AddComponent<PhysicsBody>(bodyDesc);

// BoxColliderを追加
BoxColliderDesc boxDesc;
boxDesc.SizeX = 2.0f; // X方向のサイズ
boxDesc.SizeY = 1.0f; // Y方向のサイズ
boxDesc.SizeZ = 3.0f; // Z方向のサイズ
boxDesc.OffsetPos = Vector3(0.0f, 0.5f, 0.0f); // オフセット位置
boxDesc.Friction = 0.5f;    // 摩擦係数
boxDesc.Restitution = 0.3f; // 反発係数
obj->AddComponent<ColliderShape>(boxDesc);
```

---

### 2. `SphereCollider`（球）
球体の形状です。

**使い方:**
```cpp
GameObject* obj = scene.CreateGameObject();

// PhysicsBodyを追加
PhysicsBodyDesc bodyDesc;
bodyDesc.Type = BodyType::DYNAMIC;
bodyDesc.Mass = 5.0f;
obj->AddComponent<PhysicsBody>(bodyDesc);

// SphereColliderを追加
SphereColliderDesc sphereDesc;
sphereDesc.Radius = 1.0f; // 半径
sphereDesc.Friction = 0.3f;
sphereDesc.Restitution = 0.8f; // よく弾む
obj->AddComponent<ColliderShape>(sphereDesc);
```

---

### 3. `CapsuleCollider`（カプセル）
カプセル形状です。キャラクターコントローラーに最適です。

**使い方:**
```cpp
GameObject* player = scene.CreateGameObject();

// PhysicsBodyを追加
PhysicsBodyDesc bodyDesc;
bodyDesc.Type = BodyType::DYNAMIC;
bodyDesc.Mass = 10.0f;
bodyDesc.FixedRotation = Vector3(1.0f, 0.0f, 1.0f); // X,Z軸回転を固定
player->AddComponent<PhysicsBody>(bodyDesc);

// CapsuleColliderを追加
CapsuleColliderDesc capsuleDesc;
capsuleDesc.Radius = 0.5f;  // 半径
capsuleDesc.Height = 2.0f;  // 高さ
capsuleDesc.Friction = 0.0f; // 滑らかに移動
obj->AddComponent<ColliderShape>(capsuleDesc);
```

---

### 4. `CylinderCollider`（シリンダー）
円柱形状です。

**使い方:**
```cpp
GameObject* obj = scene.CreateGameObject();

// PhysicsBodyを追加
PhysicsBodyDesc bodyDesc;
bodyDesc.Type = BodyType::STATIC;
obj->AddComponent<PhysicsBody>(bodyDesc);

// CylinderColliderを追加
CylinderColliderDesc cylinderDesc;
cylinderDesc.Radius = 1.0f;  // 半径
cylinderDesc.Height = 3.0f;  // 高さ
obj->AddComponent<ColliderShape>(cylinderDesc);
```

---

## ColliderShapeDesc（コライダー形状の設定）

全てのコライダー形状で共通の設定項目：

```cpp
struct ColliderShapeDesc
{
    Vector3 OffsetPos = {0.0f, 0.0f, 0.0f};    // オフセット位置
    Quaternion OffsetRot = Quaternion::Identity(); // オフセット回転
    float Friction = 0.5f;      // 摩擦係数（0.0～1.0）
    float Restitution = 0.0f;   // 反発係数（0.0～1.0）
};
```

---

## 実用例

### プレイヤーの設定
```cpp
class PlayerSetup : public ScriptComponent
{
public:
    void Awake() override
    {
        GameObject& obj = gameObject();
        
        // PhysicsBodyの設定
        PhysicsBodyDesc bodyDesc;
        bodyDesc.Type = BodyType::DYNAMIC;
        bodyDesc.Mass = 70.0f; // 70kg
        bodyDesc.Gravity = Vector3(0.0f, -9.81f, 0.0f);
        bodyDesc.FixedRotation = Vector3(1.0f, 0.0f, 1.0f); // X,Z軸回転を固定
        obj.AddComponent<PhysicsBody>(bodyDesc);
        
        // CapsuleColliderの設定
        CapsuleColliderDesc capsuleDesc;
        capsuleDesc.Radius = 0.5f;
        capsuleDesc.Height = 1.8f;
        capsuleDesc.OffsetPos = Vector3(0.0f, 0.9f, 0.0f); // 足元を中心に
        capsuleDesc.Friction = 0.0f; // 滑らかに移動
        obj.AddComponent<ColliderShape>(capsuleDesc);
    }
};
```

---

### 弾丸の設定
```cpp
class BulletSetup : public ScriptComponent
{
public:
    void Awake() override
    {
        GameObject& obj = gameObject();
        
        // PhysicsBodyの設定
        PhysicsBodyDesc bodyDesc;
        bodyDesc.Type = BodyType::DYNAMIC;
        bodyDesc.Mass = 0.1f; // 軽い
        bodyDesc.Gravity = Vector3(0.0f, 0.0f, 0.0f); // 重力なし
        obj.AddComponent<PhysicsBody>(bodyDesc);
        
        // SphereColliderの設定
        SphereColliderDesc sphereDesc;
        sphereDesc.Radius = 0.1f;
        sphereDesc.Restitution = 0.0f; // 弾まない
        obj.AddComponent<ColliderShape>(sphereDesc);
    }
    
    void Start() override
    {
        // 前方に発射
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (body)
        {
            body->SetVelocity(Vector3(0.0f, 0.0f, 20.0f));
        }
    }
};
```

---

### 床・壁の設定
```cpp
class GroundSetup : public ScriptComponent
{
public:
    void Awake() override
    {
        GameObject& obj = gameObject();
        
        // PhysicsBodyの設定（静的）
        PhysicsBodyDesc bodyDesc;
        bodyDesc.Type = BodyType::STATIC;
        obj.AddComponent<PhysicsBody>(bodyDesc);
        
        // BoxColliderの設定
        BoxColliderDesc boxDesc;
        boxDesc.SizeX = 50.0f;
        boxDesc.SizeY = 1.0f;
        boxDesc.SizeZ = 50.0f;
        boxDesc.Friction = 0.8f; // 摩擦が高い
        obj.AddComponent<ColliderShape>(boxDesc);
    }
};
```

---

### ジャンプ処理
```cpp
class JumpController : public ScriptComponent
{
private:
    bool m_isGrounded = false;
    
public:
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (!body) return;
        
        // 地面との衝突をチェック
        auto collisions = body->GetCollisionStay();
        m_isGrounded = false;
        
        for (auto* other : collisions)
        {
            if (other->gameObject().CompareTag("Ground"))
            {
                m_isGrounded = true;
                break;
            }
        }
        
        // スペースキーでジャンプ
        if (input().keyboard().GetKeyDown(VK_SPACE) && m_isGrounded)
        {
            Vector3 jumpImpulse = Vector3(0.0f, 500.0f, 0.0f);
            body->ApplyImpulse(jumpImpulse);
        }
    }
};
```

---

### トリガーゾーン（ダメージエリア）
```cpp
class DamageZone : public ScriptComponent
{
public:
    void Awake() override
    {
        GameObject& obj = gameObject();
        
        // トリガーとして設定
        PhysicsBodyDesc bodyDesc;
        bodyDesc.Type = BodyType::STATIC;
        bodyDesc.IsTrigger = true; // トリガー有効
        obj.AddComponent<PhysicsBody>(bodyDesc);
        
        // エリアの形状
        BoxColliderDesc boxDesc;
        boxDesc.SizeX = 5.0f;
        boxDesc.SizeY = 2.0f;
        boxDesc.SizeZ = 5.0f;
        obj.AddComponent<ColliderShape>(boxDesc);
    }
    
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (!body) return;
        
        // トリガー内のオブジェクトを検出
        auto triggers = body->GetTriggerStay();
        for (auto* other : triggers)
        {
            GameObject& otherObj = other->gameObject();
            
            if (otherObj.CompareTag("Player"))
            {
                // プレイヤーにダメージ
                auto* health = otherObj.GetComponent<HealthComponent>();
                if (health)
                {
                    health->TakeDamage(10.0f * Time::DeltaTime());
                }
            }
        }
    }
};
```

---

### キャラクターコントローラー
```cpp
class CharacterController : public ScriptComponent
{
private:
    float m_moveSpeed = 5.0f;
    
public:
    void Awake() override
    {
        GameObject& obj = gameObject();
        
        // PhysicsBodyの設定
        PhysicsBodyDesc bodyDesc;
        bodyDesc.Type = BodyType::DYNAMIC;
        bodyDesc.Mass = 70.0f;
        bodyDesc.FixedRotation = Vector3(1.0f, 0.0f, 1.0f); // X,Z軸回転固定
        obj.AddComponent<PhysicsBody>(bodyDesc);
        
        // CapsuleCollider
        CapsuleColliderDesc capsuleDesc;
        capsuleDesc.Radius = 0.5f;
        capsuleDesc.Height = 1.8f;
        capsuleDesc.OffsetPos = Vector3(0.0f, 0.9f, 0.0f);
        obj.AddComponent<ColliderShape>(capsuleDesc);
    }
    
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (!body) return;
        
        // 入力を取得
        Vector3 moveDirection = Vector3::Zero();
        
        if (input().keyboard().GetKey(VK_W)) moveDirection.z += 1.0f;
        if (input().keyboard().GetKey(VK_S)) moveDirection.z -= 1.0f;
        if (input().keyboard().GetKey(VK_A)) moveDirection.x -= 1.0f;
        if (input().keyboard().GetKey(VK_D)) moveDirection.x += 1.0f;
        
        if (moveDirection.Length() > 0.0f)
        {
            moveDirection = moveDirection.Normalize();
            
            // 現在の速度を取得
            Vector3 velocity = body->GetVelocity();
            
            // 水平方向の速度を設定
            velocity.x = moveDirection.x * m_moveSpeed;
            velocity.z = moveDirection.z * m_moveSpeed;
            
            body->SetVelocity(velocity);
        }
    }
};
```

---

## 重要なポイント

### BodyType の使い分け
- **STATIC**: 動かない地形・障害物
- **DYNAMIC**: 重力や力の影響を受ける動くオブジェクト
- **KINEMATIC**: スクリプトで制御する動くオブジェクト

### Collision と Trigger の違い
- **Collision**: 物理的に押し出される
- **Trigger**: 検出だけ行う（押し出しなし）

### ColliderShape は必須
`PhysicsBody` だけでは衝突判定は機能しません。必ず `ColliderShape` も追加してください。

### 質量と重力
- `BodyType::DYNAMIC` の場合のみ質量と重力が有効
- 質量が大きいほど動かしにくい

### 固定回転
`FixedRotation` を使うと、キャラクターが倒れないようにできます：
```cpp
bodyDesc.FixedRotation = Vector3(1.0f, 0.0f, 1.0f); // X,Z軸回転を固定
```

---

## まとめ
`PhysicsBody` と `ColliderShape` を組み合わせることで、オブジェクトに物理的な振る舞いと衝突判定を持たせることができます。`BodyType` を適切に選び、`ColliderShape` で形状を定義し、`ApplyForce()` / `ApplyImpulse()` で力を加え、`GetCollisionEnter()` / `GetTriggerEnter()` で衝突を検出します。これらを使いこなすことで、リアルな物理シミュレーションとインタラクションを実装できます。
