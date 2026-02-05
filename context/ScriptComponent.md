# `ScriptComponent` クラスの使い方

## 概要
`ScriptComponent` は `Component` を継承したクラスで、ユーザーがゲームロジックを実装するための専用コンポーネントです。毎フレーム呼ばれる更新メソッドを提供し、プレイヤー制御、敵AI、ゲームルールなどを実装します。

---

## ライフサイクルメソッド

`ScriptComponent` は以下の順序でメソッドが呼ばれます：

### 1. `void Awake()`
`ScriptComponent` が追加された直後に**一度だけ**呼ばれます。初期化処理を記述します。

**使い方:**
```cpp
class PlayerController : public ScriptComponent
{
private:
    float m_speed = 5.0f;
    
public:
    void Awake() override
    {
        // コンポーネント追加時の初期化
        std::cout << "PlayerController Awake!" << std::endl;
        
        // 初期設定
        gameObject().SetTag("Player");
    }
};
```

---

### 2. `void Start()`
`Awake()` の後、最初の `Update()` が呼ばれる前に**一度だけ**呼ばれます。他のコンポーネントが初期化済みであることが保証されます。

**使い方:**
```cpp
class PlayerController : public ScriptComponent
{
public:
    void Start() override
    {
        // 他のコンポーネントを取得して初期化
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (body)
        {
            body->SetMass(10.0f);
        }
        
        std::cout << "PlayerController Start!" << std::endl;
    }
};
```

---

### 3. `void PreUpdate()`
毎フレーム、`Update()` の**前に**呼ばれます。入力処理などを記述します。

**使い方:**
```cpp
class PlayerController : public ScriptComponent
{
private:
    Vector3 m_inputDirection = Vector3::Zero();
    
public:
    void PreUpdate() override
    {
        // 入力の取得（Update前に実行）
        m_inputDirection = Vector3::Zero();
        
        if (input().GetKey(KEY_W))
            m_inputDirection.z += 1.0f;
        if (input().GetKey(KEY_S))
            m_inputDirection.z -= 1.0f;
        if (input().GetKey(KEY_A))
            m_inputDirection.x -= 1.0f;
        if (input().GetKey(KEY_D))
            m_inputDirection.x += 1.0f;
    }
};
```

---

### 4. `void Update()`
毎フレーム呼ばれるメインの更新処理です。ゲームロジックを記述します。

**使い方:**
```cpp
class PlayerController : public ScriptComponent
{
private:
    Vector3 m_inputDirection = Vector3::Zero();
    float m_speed = 5.0f;
    
public:
    void Update() override
    {
        // 移動処理
        if (m_inputDirection.LengthSquared() > 0.0f)
        {
            m_inputDirection.Normalize();
            Vector3 movement = m_inputDirection * m_speed * Time::deltaTime();
            gameObject().transform().Translate(movement);
        }
        
        // ジャンプ処理
        if (input().GetKeyDown(KEY_SPACE))
        {
            auto* body = gameObject().GetComponent<PhysicsBody>();
            if (body)
            {
                body->AddImpulse(Vector3(0.0f, 10.0f, 0.0f));
            }
        }
    }
};
```

---

### 5. `void PostUpdate()`
毎フレーム、`Update()` の**後に**呼ばれます。カメラ追従などの後処理を記述します。

**使い方:**
```cpp
class CameraFollow : public ScriptComponent
{
private:
    GameObject* m_target = nullptr;
    Vector3 m_offset = Vector3(0.0f, 5.0f, -10.0f);
    
public:
    void Start() override
    {
        // 追従対象を検索
        m_target = GetGameObjectByTag("Player");
    }
    
    void PostUpdate() override
    {
        // プレイヤーの移動後にカメラを追従させる
        if (m_target)
        {
            Vector3 targetPos = m_target->transform().GetPosition();
            gameObject().transform().SetPosition(targetPos + m_offset);
        }
    }
};
```

---

### 6. `void OnDestroy()`
`ScriptComponent` が破棄される直前に**一度だけ**呼ばれます。クリーンアップ処理を記述します。

**使い方:**
```cpp
class EnemyAI : public ScriptComponent
{
public:
    void OnDestroy() override
    {
        // 破棄時の処理
        std::cout << "Enemy destroyed!" << std::endl;
        
        // スコアの加算など
    }
};
```

---

## 実行順序のまとめ

```
1. Awake()          ← 追加時に一度だけ
2. Start()          ← 最初のフレームの前に一度だけ
3. PreUpdate()      ← 毎フレーム
4. Update()         ← 毎フレーム
5. PostUpdate()     ← 毎フレーム
   (3~5を繰り返す)
6. OnDestroy()      ← 破棄時に一度だけ
```

---

## 使用例

### プレイヤー制御スクリプト
```cpp
class PlayerController : public ScriptComponent
{
private:
    float m_speed = 5.0f;
    float m_jumpPower = 10.0f;
    Vector3 m_moveDirection = Vector3::Zero();
    
public:
    void Awake() override
    {
        gameObject().SetTag("Player");
    }
    
    void Start() override
    {
        // 物理ボディの初期設定
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (body)
        {
            body->SetMass(10.0f);
        }
    }
    
    void PreUpdate() override
    {
        // 入力の取得
        m_moveDirection = Vector3::Zero();
        
        if (input().GetKey(KEY_W)) m_moveDirection.z += 1.0f;
        if (input().GetKey(KEY_S)) m_moveDirection.z -= 1.0f;
        if (input().GetKey(KEY_A)) m_moveDirection.x -= 1.0f;
        if (input().GetKey(KEY_D)) m_moveDirection.x += 1.0f;
    }
    
    void Update() override
    {
        // 移動
        if (m_moveDirection.LengthSquared() > 0.0f)
        {
            m_moveDirection.Normalize();
            Vector3 movement = m_moveDirection * m_speed * Time::deltaTime();
            gameObject().transform().Translate(movement);
        }
        
        // ジャンプ
        if (input().GetKeyDown(KEY_SPACE))
        {
            auto* body = gameObject().GetComponent<PhysicsBody>();
            if (body)
            {
                body->AddImpulse(Vector3(0.0f, m_jumpPower, 0.0f));
            }
        }
    }
    
    void OnDestroy() override
    {
        std::cout << "Player destroyed!" << std::endl;
    }
};

// 使用例
GameObject* player = scene.CreateGameObject();
player->AddComponent<PlayerController>();
```

---

### 敵AIスクリプト
```cpp
class EnemyAI : public ScriptComponent
{
private:
    GameObject* m_player = nullptr;
    float m_speed = 3.0f;
    float m_detectionRange = 10.0f;
    
public:
    void Start() override
    {
        // プレイヤーを検索
        m_player = GetGameObjectByTag("Player");
    }
    
    void Update() override
    {
        if (!m_player) return;
        
        // プレイヤーとの距離を計算
        Vector3 myPos = gameObject().transform().GetPosition();
        Vector3 playerPos = m_player->transform().GetPosition();
        Vector3 direction = playerPos - myPos;
        float distance = direction.Length();
        
        // 範囲内ならプレイヤーを追いかける
        if (distance < m_detectionRange)
        {
            direction.Normalize();
            Vector3 movement = direction * m_speed * Time::deltaTime();
            gameObject().transform().Translate(movement);
        }
    }
    
    void OnDestroy() override
    {
        // スコア加算などの処理
        std::cout << "Enemy defeated! +100 points" << std::endl;
    }
};

// 使用例
GameObject* enemy = scene.CreateGameObject();
enemy->SetTag("Enemy");
enemy->AddComponent<EnemyAI>();
```

---

### 回転オブジェクトスクリプト
```cpp
class RotateObject : public ScriptComponent
{
private:
    float m_rotationSpeed = 90.0f; // 度/秒
    
public:
    void Update() override
    {
        // Y軸周りに回転
        Transform& trans = gameObject().transform();
        Quaternion rotation = trans.GetRotation();
        
        float angleRad = m_rotationSpeed * Time::deltaTime() * (3.14159f / 180.0f);
        Quaternion deltaRotation = Quaternion::FromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), angleRad);
        
        trans.SetRotation(rotation * deltaRotation);
    }
};

// 使用例
GameObject* coin = scene.CreateGameObject();
coin->AddComponent<RotateObject>();
```

---

### タイマースクリプト
```cpp
class TimedDestroy : public ScriptComponent
{
private:
    float m_lifetime = 5.0f;
    float m_elapsedTime = 0.0f;
    
public:
    TimedDestroy(float lifetime) : m_lifetime(lifetime) {}
    
    void Update() override
    {
        m_elapsedTime += Time::deltaTime();
        
        if (m_elapsedTime >= m_lifetime)
        {
            // 時間が経過したら自身を破棄
            gameObject().Destroy();
        }
    }
    
    void OnDestroy() override
    {
        std::cout << "Object destroyed after " << m_lifetime << " seconds!" << std::endl;
    }
};

// 使用例
GameObject* bullet = scene.CreateGameObject();
bullet->AddComponent<TimedDestroy>(3.0f); // 3秒後に破棄
```

---

## まとめ
`ScriptComponent` はゲームロジックを実装するための専用コンポーネントです。`Awake()` / `Start()` で初期化し、`PreUpdate()` / `Update()` / `PostUpdate()` で毎フレームの処理を記述し、`OnDestroy()` でクリーンアップを行います。プレイヤー操作、敵AI、ゲームルールなど、あらゆるゲームの振る舞いを実装できます。
