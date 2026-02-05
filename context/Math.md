# `Vector3` と `Quaternion` の使い方

## 概要
`Vector3` は3次元空間の位置や方向を表すクラス、`Quaternion` は回転を表すクラスです。両方とも DirectX Math を内部で使用しており、ゲーム開発で頻繁に使用されます。

---

# `Vector3` クラス

## 基本的な使い方

### コンストラクタ
```cpp
// デフォルトコンストラクタ (0, 0, 0)
Vector3 zero = Vector3();

// 成分を指定
Vector3 position = Vector3(1.0f, 2.0f, 3.0f);

// x, y, z に直接アクセス
position.x = 5.0f;
position.y = 10.0f;
position.z = 15.0f;
```

---

## 算術演算

### 加算・減算
```cpp
Vector3 a = Vector3(1.0f, 2.0f, 3.0f);
Vector3 b = Vector3(4.0f, 5.0f, 6.0f);

// ベクトルの加算
Vector3 sum = a + b; // (5, 7, 9)

// ベクトルの減算
Vector3 diff = b - a; // (3, 3, 3)

// 累積代入演算子
a += b; // a = (5, 7, 9)
a -= b; // a = (1, 2, 3)
```

---

### スカラー倍
```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);

// スカラー倍
Vector3 scaled = vec * 2.0f; // (2, 4, 6)

// 累積代入演算子
vec *= 3.0f; // vec = (3, 6, 9)
```

---

## ベクトルの長さと正規化

### `float Length()`
ベクトルの長さ（大きさ）を計算します。

```cpp
Vector3 vec = Vector3(3.0f, 4.0f, 0.0f);
float length = vec.Length(); // 5.0
```

---

### `Vector3 Normalize()`
ベクトルを正規化（長さを1にする）します。方向だけを保持したい場合に使用します。

```cpp
Vector3 vec = Vector3(3.0f, 4.0f, 0.0f);
Vector3 normalized = vec.Normalize(); // (0.6, 0.8, 0)

// 使用例：移動方向の正規化
Vector3 direction = targetPos - currentPos;
direction = direction.Normalize();
Vector3 movement = direction * speed * Time::DeltaTime();
```

---

### `bool IsZero()`
ベクトルが零ベクトル（0, 0, 0）かどうかを判定します。

```cpp
Vector3 vec = Vector3(0.0f, 0.0f, 0.0f);
if (vec.IsZero())
{
    std::cout << "This is a zero vector" << std::endl;
}
```

---

## 内積と外積

### `float Dot(const Vector3& other)`
内積（ドット積）を計算します。2つのベクトルがどれだけ同じ方向を向いているかを判定できます。

```cpp
Vector3 a = Vector3(1.0f, 0.0f, 0.0f);
Vector3 b = Vector3(0.0f, 1.0f, 0.0f);

float dot = a.Dot(b); // 0.0（垂直）

// 使用例：前方向かどうかの判定
Vector3 forward = Vector3(0.0f, 0.0f, 1.0f);
Vector3 toTarget = (targetPos - currentPos).Normalize();

float dot = forward.Dot(toTarget);
if (dot > 0.7f) // 角度が約45度以内
{
    std::cout << "Target is in front" << std::endl;
}
```

---

### `float Cross(const Vector3& other)`
外積のスカラー値を返します。

---

### `Vector3 CrossVector(const Vector3& other)`
外積ベクトルを計算します。2つのベクトルに垂直なベクトルを得られます。

```cpp
Vector3 a = Vector3(1.0f, 0.0f, 0.0f);
Vector3 b = Vector3(0.0f, 1.0f, 0.0f);

Vector3 cross = a.CrossVector(b); // (0, 0, 1)（Z軸方向）
```

---

## 回転

### `Vector3 Rotate(const Quaternion& quat)`
クォータニオンによる回転を適用します。

```cpp
Vector3 vec = Vector3(1.0f, 0.0f, 0.0f);
Quaternion rotation = Quaternion::SetEulerY(90.0f * 3.14159f / 180.0f); // Y軸周りに90度

Vector3 rotated = vec.Rotate(rotation); // (0, 0, -1)
```

---

### `Vector3 RotateAxis(const Vector3& axis, float angle)`
任意の軸周りに回転します。

```cpp
Vector3 vec = Vector3(1.0f, 0.0f, 0.0f);
Vector3 axis = Vector3(0.0f, 1.0f, 0.0f); // Y軸
float angle = 90.0f * 3.14159f / 180.0f; // 90度（ラジアン）

Vector3 rotated = vec.RotateAxis(axis, angle); // (0, 0, -1)
```

---

## DirectX Math との変換

### `DirectX::XMFLOAT3 ToXMFLOAT3()`
DirectX の `XMFLOAT3` に変換します。

```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
DirectX::XMFLOAT3 xmFloat = vec.ToXMFLOAT3();
```

---

### `DirectX::XMVECTOR ToXMVECTOR()`
DirectX の `XMVECTOR` に変換します。

```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
DirectX::XMVECTOR xmVec = vec.ToXMVECTOR();
```

---

### `void FromXMFLOAT3(const DirectX::XMFLOAT3& vec)`
`XMFLOAT3` から `Vector3` に変換します。

```cpp
DirectX::XMFLOAT3 xmFloat = DirectX::XMFLOAT3(1.0f, 2.0f, 3.0f);
Vector3 vec;
vec.FromXMFLOAT3(xmFloat);
```

---

### `void FromXMVECTOR(const DirectX::XMVECTOR& vec)`
`XMVECTOR` から `Vector3` に変換します。

```cpp
DirectX::XMVECTOR xmVec = DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
Vector3 vec;
vec.FromXMVECTOR(xmVec);
```

---

## 実用例

### オブジェクトの移動
```cpp
class MoveToTarget : public ScriptComponent
{
private:
    Vector3 m_targetPosition = Vector3(10.0f, 0.0f, 10.0f);
    float m_speed = 5.0f;
    
public:
    void Update() override
    {
        Vector3 currentPos = gameObject().transform().GetPosition();
        
        // ターゲットへの方向ベクトル
        Vector3 direction = m_targetPosition - currentPos;
        
        // 距離をチェック
        if (direction.Length() > 0.1f)
        {
            // 正規化して移動
            direction = direction.Normalize();
            Vector3 movement = direction * m_speed * Time::DeltaTime();
            gameObject().transform().Translate(movement);
        }
    }
};
```

---

### 敵AIの視界判定
```cpp
class EnemyVision : public ScriptComponent
{
public:
    bool CanSeeTarget(GameObject* target)
    {
        Vector3 myPos = gameObject().transform().GetPosition();
        Vector3 targetPos = target->transform().GetPosition();
        Vector3 myForward = Vector3(0.0f, 0.0f, 1.0f); // 前方向
        
        // ターゲットへの方向
        Vector3 toTarget = (targetPos - myPos).Normalize();
        
        // 内積で角度をチェック（視野角45度 = cos(45°) ? 0.7）
        float dot = myForward.Dot(toTarget);
        if (dot > 0.7f)
        {
            return true; // 視界内
        }
        return false;
    }
};
```

---

# `Quaternion` クラス

## 概要
`Quaternion` は回転を表現するクラスです。オイラー角（Pitch, Yaw, Roll）と比べてジンバルロックを避けられ、補間がスムーズです。

---

## 基本的な使い方

### コンストラクタ
```cpp
// デフォルトコンストラクタ
Quaternion quat = Quaternion();

// Vector4から作成
Vector4 vec = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
Quaternion quat = Quaternion(vec);
```

---

## 回転の作成

### `static Quaternion Identity()`
単位クォータニオン（回転なし）を取得します。

```cpp
Quaternion noRotation = Quaternion::Identity();
```

---

### `static Quaternion SetEulerX(float angle)`
X軸周りの回転を作成します。

```cpp
float angle = 90.0f * 3.14159f / 180.0f; // 90度（ラジアン）
Quaternion rotX = Quaternion::SetEulerX(angle);
```

---

### `static Quaternion SetEulerY(float angle)`
Y軸周りの回転を作成します。

```cpp
float angle = 45.0f * 3.14159f / 180.0f; // 45度
Quaternion rotY = Quaternion::SetEulerY(angle);
```

---

### `static Quaternion SetEulerZ(float angle)`
Z軸周りの回転を作成します。

```cpp
float angle = 30.0f * 3.14159f / 180.0f; // 30度
Quaternion rotZ = Quaternion::SetEulerZ(angle);
```

---

### `static Quaternion SetAngleAxis(float angle, const Vector3& axis)`
任意の軸周りの回転を作成します。

```cpp
Vector3 axis = Vector3(0.0f, 1.0f, 0.0f); // Y軸
float angle = 90.0f * 3.14159f / 180.0f;

Quaternion rotation = Quaternion::SetAngleAxis(angle, axis);
```

---

### `static Quaternion SetRollPitchYaw(float roll, float pitch, float yaw)`
ロール・ピッチ・ヨーの順で回転を作成します。

```cpp
float roll = 0.0f;
float pitch = 45.0f * 3.14159f / 180.0f;
float yaw = 90.0f * 3.14159f / 180.0f;

Quaternion rotation = Quaternion::SetRollPitchYaw(roll, pitch, yaw);
```

---

## クォータニオンの合成

### `Quaternion operator*(const Quaternion& other)`
2つの回転を合成します。順序が重要です（左から右へ適用）。

```cpp
// Y軸周りに90度回転した後、X軸周りに45度回転
Quaternion rotY = Quaternion::SetEulerY(90.0f * 3.14159f / 180.0f);
Quaternion rotX = Quaternion::SetEulerX(45.0f * 3.14159f / 180.0f);

Quaternion combined = rotY * rotX;
```

---

### `Quaternion& operator*=(const Quaternion& other)`
累積代入演算子です。

```cpp
Quaternion rotation = Quaternion::Identity();
rotation *= Quaternion::SetEulerY(45.0f * 3.14159f / 180.0f);
```

---

## DirectX Math との変換

### `DirectX::XMFLOAT4 ToXMFLOAT4()`
DirectX の `XMFLOAT4` に変換します。

### `DirectX::XMVECTOR ToXMVECTOR()`
DirectX の `XMVECTOR` に変換します。

### `DirectX::XMMATRIX ToXMMATRIX()`
回転行列に変換します。

### `void FromXMFLOAT4(const DirectX::XMFLOAT4& vec)`
`XMFLOAT4` から変換します。

### `void FromXMVECTOR(const DirectX::XMVECTOR& vec)`
`XMVECTOR` から変換します。

### `void FromXMMATRIX(const DirectX::XMMATRIX& mat)`
回転行列から変換します。

---

## 実用例

### オブジェクトの回転
```cpp
class RotateObject : public ScriptComponent
{
private:
    float m_rotationSpeed = 90.0f; // 度/秒
    
public:
    void Update() override
    {
        Transform& trans = gameObject().transform();
        Quaternion currentRotation = trans.GetRotation();
        
        // Y軸周りに回転
        float angleRad = m_rotationSpeed * Time::DeltaTime() * (3.14159f / 180.0f);
        Quaternion deltaRotation = Quaternion::SetEulerY(angleRad);
        
        // 回転を合成
        trans.SetRotation(currentRotation * deltaRotation);
    }
};
```

---

### ターゲットを向く
```cpp
class LookAtTarget : public ScriptComponent
{
private:
    GameObject* m_target = nullptr;
    
public:
    void Start() override
    {
        m_target = GetGameObjectByTag("Player");
    }
    
    void Update() override
    {
        if (!m_target) return;
        
        Vector3 myPos = gameObject().transform().GetPosition();
        Vector3 targetPos = m_target->transform().GetPosition();
        
        // ターゲットへの方向ベクトル
        Vector3 direction = (targetPos - myPos).Normalize();
        
        // Y軸周りの回転角度を計算
        float angle = atan2f(direction.x, direction.z);
        
        // クォータニオンを作成して設定
        Quaternion rotation = Quaternion::SetEulerY(angle);
        gameObject().transform().SetRotation(rotation);
    }
};
```

---

### 任意の軸周りに回転
```cpp
class RotateAroundAxis : public ScriptComponent
{
public:
    void Update() override
    {
        Transform& trans = gameObject().transform();
        
        // 斜めの軸周りに回転
        Vector3 axis = Vector3(1.0f, 1.0f, 0.0f).Normalize();
        float angle = 45.0f * Time::DeltaTime() * (3.14159f / 180.0f);
        
        Quaternion deltaRotation = Quaternion::SetAngleAxis(angle, axis);
        Quaternion currentRotation = trans.GetRotation();
        
        trans.SetRotation(currentRotation * deltaRotation);
    }
};
```

---

## まとめ

### `Vector3` の用途
- 位置（Position）
- 方向（Direction）
- 速度（Velocity）
- 加速度（Acceleration）
- スケール（Scale）

### `Quaternion` の用途
- 回転（Rotation）
- 向きの変更
- 滑らかな回転補間

### 重要なポイント
- **Vector3**: 長さや方向を扱う場合は `Normalize()` を使う
- **Quaternion**: 回転を合成する場合は `*` 演算子を使う
- **角度**: ラジアンで指定（度→ラジアン: `angle * 3.14159f / 180.0f`）

これらのクラスを使いこなすことで、3D空間での移動・回転・方向判定などを柔軟に実装できます。
