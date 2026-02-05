# `Time` クラスの使い方

## 概要
`Time` クラスはゲームの時間管理を行う静的クラスです。フレーム間の経過時間（デルタタイム）の取得、時間スケールの変更、固定更新間隔の管理などを提供します。

---

## 基本的な関数

### 1. `static double DeltaTime()`
前回のフレームから経過した時間（秒）を取得します。移動や回転など、フレームレート非依存の処理に使用します。

**使い方:**
```cpp
class PlayerController : public ScriptComponent
{
private:
    float m_speed = 5.0f;
    
public:
    void Update() override
    {
        // デルタタイムを使ってフレームレート非依存の移動
        float moveDistance = m_speed * Time::DeltaTime();
        
        Vector3 movement = Vector3(1.0f, 0.0f, 0.0f) * moveDistance;
        gameObject().transform().Translate(movement);
    }
};
```

**重要:** `Time::DeltaTime()` は `TimeScale` の影響を受けます。

---

### 2. `static double FixedDeltaTime()`
固定更新の間隔（秒）を取得します。物理演算など、一定の時間間隔で実行したい処理に使用します。

**使い方:**
```cpp
// デフォルトは 1/60 秒 (60 FPS)
double fixedDelta = Time::FixedDeltaTime();
std::cout << "Fixed delta time: " << fixedDelta << " seconds" << std::endl;
```

---

### 3. `static float TimeScale()`
現在の時間スケールを取得します。デフォルトは `1.0f`（通常速度）です。

**使い方:**
```cpp
float currentScale = Time::TimeScale();
std::cout << "Current time scale: " << currentScale << std::endl;
```

---

### 4. `static void SetTimeScale(float scale)`
時間の流れる速度を変更します。スローモーション、早送り、一時停止などに使用します。

**使い方:**
```cpp
class GameManager : public ScriptComponent
{
public:
    void Update() override
    {
        // スローモーションエフェクト (50%の速度)
        if (input().GetKeyDown(KEY_1))
        {
            Time::SetTimeScale(0.5f);
        }
        
        // 通常速度に戻す
        if (input().GetKeyDown(KEY_2))
        {
            Time::SetTimeScale(1.0f);
        }
        
        // 早送り (2倍速)
        if (input().GetKeyDown(KEY_3))
        {
            Time::SetTimeScale(2.0f);
        }
        
        // 一時停止
        if (input().GetKeyDown(KEY_P))
        {
            Time::SetTimeScale(0.0f);
        }
    }
};
```

---

### 5. `static void SetFixedDeltaTime(float seconds)`
固定更新の間隔を変更します。デフォルトは `1/60` 秒です。

**使い方:**
```cpp
// 固定更新を30FPSに変更
Time::SetFixedDeltaTime(1.0f / 30.0f);

// 固定更新を120FPSに変更
Time::SetFixedDeltaTime(1.0f / 120.0f);
```

---

## 内部処理用の関数

以下の関数はゲームループの内部で使用され、通常はユーザーが直接呼び出すことはありません。

### 6. `static void BeginFrame()`
フレームの開始時に呼ばれ、デルタタイムを計算します。

### 7. `static bool HasFixedStep()`
固定更新を実行すべきかどうかを判定します。

### 8. `static void ConsumeFixedStep()`
固定更新を1ステップ消費します。

---

## 使用例

### 基本的な移動処理
```cpp
class MoveObject : public ScriptComponent
{
private:
    float m_speed = 10.0f;
    
public:
    void Update() override
    {
        // デルタタイムを使った移動（フレームレート非依存）
        float distance = m_speed * Time::DeltaTime();
        
        Vector3 movement = Vector3(1.0f, 0.0f, 0.0f) * distance;
        gameObject().transform().Translate(movement);
    }
};
```

---

### 回転処理
```cpp
class RotateObject : public ScriptComponent
{
private:
    float m_rotationSpeed = 90.0f; // 度/秒
    
public:
    void Update() override
    {
        // デルタタイムを使った回転
        float angle = m_rotationSpeed * Time::DeltaTime();
        
        Transform& trans = gameObject().transform();
        Quaternion rotation = trans.GetRotation();
        
        // Y軸周りに回転
        float angleRad = angle * (3.14159f / 180.0f);
        Quaternion deltaRotation = Quaternion::SetAngleAxis(angleRad, Vector3(0.0f, 1.0f, 0.0f));
        
        trans.SetRotation(rotation * deltaRotation);
    }
};
```

---

### タイマー処理
```cpp
class Timer : public ScriptComponent
{
private:
    float m_duration = 5.0f;
    float m_elapsedTime = 0.0f;
    
public:
    Timer(float duration) : m_duration(duration) {}
    
    void Update() override
    {
        // 経過時間を加算
        m_elapsedTime += Time::DeltaTime();
        
        if (m_elapsedTime >= m_duration)
        {
            std::cout << "Timer finished!" << std::endl;
            gameObject().Destroy();
        }
    }
};

// 使用例
GameObject* obj = scene.CreateGameObject();
obj->AddComponent<Timer>(3.0f); // 3秒後に破棄
```

---

### スローモーションエフェクト
```cpp
class SlowMotionEffect : public ScriptComponent
{
private:
    float m_effectDuration = 2.0f;
    float m_elapsedTime = 0.0f;
    bool m_isActive = false;
    
public:
    void Update() override
    {
        // スペースキーでスローモーション発動
        if (input().GetKeyDown(KEY_SPACE) && !m_isActive)
        {
            Time::SetTimeScale(0.3f); // 30%の速度
            m_isActive = true;
            m_elapsedTime = 0.0f;
        }
        
        // スローモーション中
        if (m_isActive)
        {
            // 実時間で経過時間を計測（TimeScaleの影響を受けない）
            m_elapsedTime += Time::DeltaTime() / Time::TimeScale();
            
            if (m_elapsedTime >= m_effectDuration)
            {
                // 通常速度に戻す
                Time::SetTimeScale(1.0f);
                m_isActive = false;
            }
        }
    }
};
```

---

### 加速度を持つ移動
```cpp
class AcceleratedMovement : public ScriptComponent
{
private:
    Vector3 m_velocity = Vector3::Zero();
    float m_acceleration = 5.0f;
    float m_maxSpeed = 10.0f;
    
public:
    void Update() override
    {
        // 入力に応じて速度を変更
        Vector3 inputDirection = Vector3::Zero();
        
        if (input().GetKey(KEY_W)) inputDirection.z += 1.0f;
        if (input().GetKey(KEY_S)) inputDirection.z -= 1.0f;
        if (input().GetKey(KEY_A)) inputDirection.x -= 1.0f;
        if (input().GetKey(KEY_D)) inputDirection.x += 1.0f;
        
        if (inputDirection.Length() > 0.0f)
        {
            inputDirection = inputDirection.Normalize();
            
            // 加速
            m_velocity += inputDirection * m_acceleration * Time::DeltaTime();
            
            // 最大速度で制限
            if (m_velocity.Length() > m_maxSpeed)
            {
                m_velocity = m_velocity.Normalize() * m_maxSpeed;
            }
        }
        else
        {
            // 減速
            float deceleration = 10.0f;
            float speed = m_velocity.Length();
            if (speed > 0.0f)
            {
                float newSpeed = speed - deceleration * Time::DeltaTime();
                if (newSpeed < 0.0f) newSpeed = 0.0f;
                m_velocity = m_velocity.Normalize() * newSpeed;
            }
        }
        
        // 移動
        gameObject().transform().Translate(m_velocity * Time::DeltaTime());
    }
};
```

---

### ポーズ機能
```cpp
class PauseManager : public ScriptComponent
{
private:
    bool m_isPaused = false;
    
public:
    void Update() override
    {
        // Pキーでポーズ切り替え
        if (input().GetKeyDown(KEY_P))
        {
            m_isPaused = !m_isPaused;
            
            if (m_isPaused)
            {
                // ポーズ（時間を停止）
                Time::SetTimeScale(0.0f);
                std::cout << "Game Paused" << std::endl;
            }
            else
            {
                // ポーズ解除
                Time::SetTimeScale(1.0f);
                std::cout << "Game Resumed" << std::endl;
            }
        }
    }
};
```

---

## 重要なポイント

### デルタタイムを使うべき場面
- オブジェクトの移動
- オブジェクトの回転
- アニメーションの更新
- タイマーやカウントダウン
- 速度や加速度の計算

**悪い例（フレームレート依存）:**
```cpp
void Update() override
{
    // 60FPSなら速度5、30FPSなら速度2.5になってしまう
    gameObject().transform().Translate(Vector3(5.0f, 0.0f, 0.0f));
}
```

**良い例（フレームレート非依存）:**
```cpp
void Update() override
{
    float speed = 5.0f;
    float distance = speed * Time::DeltaTime();
    gameObject().transform().Translate(Vector3(distance, 0.0f, 0.0f));
}
```

---

### TimeScale の用途
- **スローモーション**: `Time::SetTimeScale(0.5f)`
- **早送り**: `Time::SetTimeScale(2.0f)`
- **一時停止**: `Time::SetTimeScale(0.0f)`
- **通常速度**: `Time::SetTimeScale(1.0f)`

**注意:** `TimeScale` を変更すると、`DeltaTime()` の値も変化します。

---

## まとめ
`Time` クラスはゲームの時間管理を担う重要なクラスです。`DeltaTime()` を使うことでフレームレート非依存の処理を実装でき、`SetTimeScale()` でスローモーションや一時停止などの演出が可能になります。全ての移動・回転・アニメーション処理には必ず `DeltaTime()` を掛けることで、滑らかで安定したゲーム体験を提供できます。
