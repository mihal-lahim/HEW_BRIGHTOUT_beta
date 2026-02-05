# `Prefab` クラスの使い方

## 概要
`Prefab` は `GameObject` のテンプレートとして機能するクラスです。同じ構成の `GameObject` を複数生成する際に、`Prefab` を使うことでコードの重複を避け、効率的にオブジェクトを生成できます。

---

## 基本的な仕組み

`Prefab` は純粋仮想クラスであり、継承してカスタム `Prefab` を作成します。

### 仮想関数

#### `void Instantiate(GameObject& gameObject)`
`GameObject` にコンポーネントや設定を適用する仮想関数です。必ず実装する必要があります。

**使い方:**
```cpp
class EnemyPrefab : public Prefab
{
public:
    void Instantiate(GameObject& gameObject) override
    {
        // GameObjectの設定
        gameObject.SetTag("Enemy");
        
        // コンポーネントの追加
        gameObject.AddComponent<MeshRenderer>();
        gameObject.AddComponent<PhysicsBody>();
        gameObject.AddComponent<EnemyAI>();
        
        // Transform の設定
        gameObject.transform().SetScale(Vector3(1.0f, 1.0f, 1.0f));
    }
};
```

---

## 使用方法

### 1. カスタム Prefab の作成
`Prefab` を継承してカスタムクラスを作成します。

**例: 弾丸の Prefab**
```cpp
class BulletPrefab : public Prefab
{
public:
    void Instantiate(GameObject& gameObject) override
    {
        // タグの設定
        gameObject.SetTag("Bullet");
        
        // コンポーネントの追加
        auto* renderer = gameObject.AddComponent<MeshRenderer>();
        auto* body = gameObject.AddComponent<PhysicsBody>();
        auto* script = gameObject.AddComponent<BulletScript>();
        
        // 物理設定
        body->SetMass(0.5f);
        body->SetGravity(false);
        
        // スケール設定
        gameObject.transform().SetScale(Vector3(0.2f, 0.2f, 0.5f));
    }
};
```

---

### 2. Scene での Prefab の使用
`Scene::Instantiate(Prefab& prefab)` を使って `Prefab` から `GameObject` を生成します。

**使い方:**
```cpp
class GameScene : public Scene
{
public:
    void Initialize() override
    {
        // Prefabを作成
        BulletPrefab bulletPrefab;
        
        // Prefabから弾丸を生成
        GameObject* bullet = Instantiate(bulletPrefab);
        bullet->transform().SetPosition(Vector3(0.0f, 0.0f, 0.0f));
    }
};
```

---

### 3. Component での Prefab の使用
`Component::Instantiate(Prefab& prefab)` を使うこともできます。

**使い方:**
```cpp
class PlayerWeapon : public ScriptComponent
{
public:
    void Update() override
    {
        if (input().GetKeyDown(KEY_SPACE))
        {
            // 弾丸を発射
            BulletPrefab bulletPrefab;
            GameObject* bullet = Instantiate(bulletPrefab);
            
            // 弾丸の位置と速度を設定
            Vector3 spawnPos = gameObject().transform().GetPosition();
            bullet->transform().SetPosition(spawnPos);
            
            auto* body = bullet->GetComponent<PhysicsBody>();
            if (body)
            {
                body->SetVelocity(Vector3(0.0f, 0.0f, 10.0f));
            }
        }
    }
};
```

---

## 実用例

### 敵の Prefab
```cpp
class EnemyPrefab : public Prefab
{
private:
    float m_health = 100.0f;
    
public:
    EnemyPrefab(float health) : m_health(health) {}
    
    void Instantiate(GameObject& gameObject) override
    {
        // タグの設定
        gameObject.SetTag("Enemy");
        
        // コンポーネントの追加
        gameObject.AddComponent<MeshRenderer>();
        gameObject.AddComponent<PhysicsBody>();
        
        auto* ai = gameObject.AddComponent<EnemyAI>();
        auto* health = gameObject.AddComponent<HealthComponent>();
        
        // HealthComponentの初期設定
        health->SetMaxHealth(m_health);
        
        // Transform の設定
        gameObject.transform().SetScale(Vector3(1.0f, 1.0f, 1.0f));
    }
};

// 使用例
class GameScene : public Scene
{
public:
    void Initialize() override
    {
        // 異なる体力の敵を生成
        EnemyPrefab weakEnemy(50.0f);
        EnemyPrefab strongEnemy(200.0f);
        
        GameObject* enemy1 = Instantiate(weakEnemy);
        enemy1->transform().SetPosition(Vector3(5.0f, 0.0f, 0.0f));
        
        GameObject* enemy2 = Instantiate(strongEnemy);
        enemy2->transform().SetPosition(Vector3(10.0f, 0.0f, 0.0f));
    }
};
```

---

### アイテムの Prefab
```cpp
class ItemPrefab : public Prefab
{
private:
    std::string m_itemType;
    int m_value;
    
public:
    ItemPrefab(const std::string& type, int value) 
        : m_itemType(type), m_value(value) {}
    
    void Instantiate(GameObject& gameObject) override
    {
        // タグの設定
        gameObject.SetTag("Item");
        
        // コンポーネントの追加
        gameObject.AddComponent<MeshRenderer>();
        
        auto* item = gameObject.AddComponent<ItemComponent>();
        item->SetType(m_itemType);
        item->SetValue(m_value);
        
        // 回転アニメーション
        gameObject.AddComponent<RotateObject>();
        
        // スケール設定
        gameObject.transform().SetScale(Vector3(0.5f, 0.5f, 0.5f));
    }
};

// 使用例
class GameScene : public Scene
{
public:
    void Initialize() override
    {
        // コイン（10ポイント）
        ItemPrefab coinPrefab("Coin", 10);
        GameObject* coin = Instantiate(coinPrefab);
        coin->transform().SetPosition(Vector3(0.0f, 1.0f, 5.0f));
        
        // 宝石（100ポイント）
        ItemPrefab gemPrefab("Gem", 100);
        GameObject* gem = Instantiate(gemPrefab);
        gem->transform().SetPosition(Vector3(3.0f, 1.0f, 5.0f));
    }
};
```

---

### 障害物の配置
```cpp
class ObstaclePrefab : public Prefab
{
public:
    void Instantiate(GameObject& gameObject) override
    {
        gameObject.SetTag("Obstacle");
        
        gameObject.AddComponent<MeshRenderer>();
        
        auto* body = gameObject.AddComponent<PhysicsBody>();
        body->SetStatic(true); // 静的な物理オブジェクト
        
        gameObject.transform().SetScale(Vector3(2.0f, 2.0f, 2.0f));
    }
};

// 使用例（複数配置）
class GameScene : public Scene
{
public:
    void Initialize() override
    {
        ObstaclePrefab obstaclePrefab;
        
        // グリッド状に障害物を配置
        for (int x = 0; x < 10; ++x)
        {
            for (int z = 0; z < 10; ++z)
            {
                if ((x + z) % 2 == 0) // チェッカーボード状に配置
                {
                    GameObject* obstacle = Instantiate(obstaclePrefab);
                    obstacle->transform().SetPosition(Vector3(x * 3.0f, 0.0f, z * 3.0f));
                }
            }
        }
    }
};
```

---

### パーティクルエフェクトの Prefab
```cpp
class ExplosionPrefab : public Prefab
{
public:
    void Instantiate(GameObject& gameObject) override
    {
        gameObject.SetTag("Effect");
        
        // パーティクルコンポーネント
        auto* particles = gameObject.AddComponent<ParticleSystem>();
        particles->SetLifetime(2.0f);
        particles->SetEmissionRate(100);
        
        // 自動削除スクリプト
        gameObject.AddComponent<TimedDestroy>(2.0f); // 2秒後に破棄
    }
};

// 使用例
class Bullet : public ScriptComponent
{
public:
    void OnCollision(GameObject* other)
    {
        // 衝突時に爆発エフェクトを生成
        ExplosionPrefab explosionPrefab;
        GameObject* explosion = Instantiate(explosionPrefab);
        explosion->transform().SetPosition(gameObject().transform().GetPosition());
        
        // 弾丸を破棄
        gameObject().Destroy();
    }
};
```

---

## Prefab のメリット

### 1. コードの再利用
同じ構成の `GameObject` を何度も生成する際、コードを一箇所にまとめられます。

### 2. メンテナンス性の向上
`Prefab` を変更すれば、その `Prefab` から生成される全ての `GameObject` に変更が反映されます。

### 3. 動的生成が容易
ゲーム実行中に簡単にオブジェクトを生成できます（敵のスポーン、弾丸の発射など）。

### 4. パラメータ化
コンストラクタでパラメータを受け取ることで、同じ `Prefab` から異なる設定のオブジェクトを生成できます。

---

## まとめ
`Prefab` は `GameObject` のテンプレートとして機能し、`Instantiate()` メソッドで `GameObject` の構成を定義します。`Scene::Instantiate()` や `Component::Instantiate()` を使って簡単にオブジェクトを生成でき、敵・弾丸・アイテムなど、繰り返し生成されるオブジェクトの管理に最適です。
