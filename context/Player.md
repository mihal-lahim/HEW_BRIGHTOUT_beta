# Player系Componentの実装ガイド

## 概要
このプロジェクトのプレイヤーシステムは、複数のコンポーネントを組み合わせて構築されています。ステートマシン、入力処理、移動制御、変身システム、カメラ制御など、各機能がモジュール化されており、拡張性の高い設計になっています。

---

# プレイヤーシステムの構成

## 主要コンポーネント一覧

### 1. **`Player`** - プレイヤーの中核クラス
- 全てのプレイヤーコンポーネントを統括
- 各コンポーネントへの参照を保持
- ステートマシンを通じて状態管理

### 2. **`PlayerStateMachine`** - 状態管理システム
- プレイヤーの状態を管理
- 状態遷移を制御

### 3. **`PlayerMovement`** - 移動制御コンポーネント
- 地上移動、ジャンプ、電線移動などの移動処理
- 重力適用、レイキャスト処理

### 4. **`InputHandler`** - 入力処理コンポーネント
- キーボード/ゲームパッド入力を抽象化
- コマンドパターンで入力を管理

### 5. **`PlayerMorphSystem`** - 変身システム
- 人間形態と電気形態の切り替え
- 変身可能判定

### 6. **`TPSCamera`** - 三人称カメラ
- プレイヤーを追従するカメラ
- マウス/スティック入力による回転制御

### 7. **`Health`** - 体力管理コンポーネント
- 体力の設定と取得
- ダメージと回復の処理

---

# `Player` クラス

## 概要
`Player` クラスは全てのプレイヤー機能を統合する中核クラスです。各種コンポーネントへの参照を保持し、それらを連携させます。

## クラス構造

```cpp
class Player : public ScriptComponent
{
public:
    // 状態管理
    PlayerStateMachine* stateMachine = nullptr;
    
    // 入力システム
    InputHandler* inputHandler = nullptr;
    
    // 移動コンポーネント
    PlayerMovement* movement = nullptr;
    
    // 変身システム
    PlayerMorphSystem* morphSystem = nullptr;
    
    // カメラ
    TPSCamera* camera = nullptr;
    
    // 体力コンポーネント
    Health* health = nullptr;
    
    // 剛体
    PhysicsBody* physicsBody = nullptr;
    
    // メッシュレンダラー
    MeshRenderer* meshRenderer = nullptr;
    
    // モデルデータ
    MODEL* humanModel = nullptr;
    MODEL* electricModel = nullptr;
    
    void Start() override;
    void Update() override;
};
```

## 使い方

### プレイヤーの生成
```cpp
class GameScene : public Scene
{
public:
    void Initialize() override
    {
        // プレイヤーオブジェクトを生成
        GameObject* playerObj = CreateGameObject();
        playerObj->SetTag("Player");
        
        // Playerコンポーネントを追加
        auto* player = playerObj->AddComponent<Player>();
        
        // PhysicsBodyとColliderShapeを追加
        PhysicsBodyDesc bodyDesc;
        bodyDesc.Type = BodyType::DYNAMIC;
        bodyDesc.Mass = 70.0f;
        bodyDesc.FixedRotation = Vector3(1.0f, 0.0f, 1.0f);
        playerObj->AddComponent<PhysicsBody>(bodyDesc);
        
        CapsuleColliderDesc capsuleDesc;
        capsuleDesc.Radius = 0.5f;
        capsuleDesc.Height = 1.8f;
        playerObj->AddComponent<ColliderShape>(capsuleDesc);
    }
};
```

---

# `PlayerStateMachine` クラス

## 概要
プレイヤーの状態を管理するステートマシンです。状態パターンを使用して、プレイヤーの振る舞いを状態ごとに分離します。

## プレイヤーの状態

### 1. **`PlayerState_Human_Idle`** - 人間形態の待機状態
- 地上で静止している状態
- 入力に応じて歩行状態に遷移

### 2. **`PlayerState_Human_Walk`** - 人間形態の歩行状態
- 地上を移動している状態
- 入力がなくなると待機状態に戻る

### 3. **`PlayerState_Human_MidAir`** - 人間形態の空中状態
- ジャンプ中や落下中の状態
- 着地すると地上状態に遷移

### 4. **`PlayerState_Electric`** - 電気形態の状態
- 電線上を移動している状態
- 電線から離脱すると人間形態に戻る

## ステートの基本構造

```cpp
class PlayerState
{
public:
    // 状態開始時の処理
    virtual void Enter(Player& player);
    
    // 入力処理
    virtual void HandleInput(Player& player);
    
    // 更新処理
    virtual void Update(Player& player);
};
```

## ステートマシンの使い方

```cpp
class CustomPlayerState : public PlayerState
{
public:
    void Enter(Player& player) override
    {
        std::cout << "Entering custom state!" << std::endl;
    }
    
    void HandleInput(Player& player) override
    {
        // 入力に応じて処理
        if (player.inputHandler->IsIssued<CommandType::JUMP>())
        {
            // 別の状態に遷移
            player.stateMachine->ChangeState(&PlayerStates::HumanMidAir, player);
        }
    }
    
    void Update(Player& player) override
    {
        // 毎フレームの更新処理
    }
};
```

---

# `PlayerMovement` クラス

## 概要
プレイヤーの移動制御を担当するコンポーネントです。地上移動、ジャンプ、電線移動など、様々な移動パターンを実装しています。

## 主な機能

### 地上移動

#### `void Walk(float inputX, float inputZ)`
通常の歩行移動を行います。

```cpp
void PlayerState_Human_Walk::Update(Player& player)
{
    // 入力値を取得
    float inputX = player.inputHandler->GetValue<CommandType::MOVE_HORIZONTAL>();
    float inputZ = player.inputHandler->GetValue<CommandType::MOVE_VERTICAL>();
    
    // 歩行
    player.movement->Walk(inputX, inputZ);
}
```

#### `void Run(float inputX, float inputZ)`
走行移動を行います（歩行の2倍速）。

```cpp
// ダッシュボタンが押されている場合
if (player.inputHandler->IsIssued<CommandType::RUN>())
{
    player.movement->Run(inputX, inputZ);
}
```

---

### ジャンプ処理

#### `void GroundJump(float inputX, float inputZ)`
地上からのジャンプを行います。

```cpp
void PlayerState_Human_Walk::HandleInput(Player& player)
{
    // ジャンプボタンが押された
    if (player.inputHandler->IsIssued<CommandType::JUMP>())
    {
        // 入力方向を取得
        float inputX = player.inputHandler->GetValue<CommandType::MOVE_HORIZONTAL>();
        float inputZ = player.inputHandler->GetValue<CommandType::MOVE_VERTICAL>();
        
        // ジャンプ
        player.movement->GroundJump(inputX, inputZ);
        
        // 空中状態に遷移
        player.stateMachine->ChangeState(&PlayerStates::HumanMidAir, player);
    }
}
```

#### `void ElectricJump(float inputX, float inputZ)`
電線からのジャンプを行います。

```cpp
void PlayerState_Electric::HandleInput(Player& player)
{
    // ジャンプで電線から離脱
    if (player.inputHandler->IsIssued<CommandType::JUMP>())
    {
        float inputX = player.inputHandler->GetValue<CommandType::MOVE_HORIZONTAL>();
        float inputZ = player.inputHandler->GetValue<CommandType::MOVE_VERTICAL>();
        
        player.movement->ElectricJump(inputX, inputZ);
        player.stateMachine->ChangeState(&PlayerStates::HumanMidAir, player);
    }
}
```

---

### 地面判定

#### `bool IsOnGround()`
プレイヤーが地面に接地しているかを判定します。

```cpp
void PlayerState_Human_MidAir::Update(Player& player)
{
    // 着地判定
    if (player.movement->IsOnGround())
    {
        // 待機状態に遷移
        player.stateMachine->ChangeState(&PlayerStates::HumanIdle, player);
    }
}
```

---

### 重力処理

#### `void ApplyGravity()`
プレイヤーに重力を適用します。

```cpp
void PlayerState_Human_MidAir::Update(Player& player)
{
    // 重力を適用
    player.movement->ApplyGravity();
    
    // 空中での移動制御
    float inputX = player.inputHandler->GetValue<CommandType::MOVE_HORIZONTAL>();
    float inputZ = player.inputHandler->GetValue<CommandType::MOVE_VERTICAL>();
    
    // 空中移動（制限付き）
    // ...
}
```

---

### 電線移動

#### `void SnapToPowerLine(PowerLineID lineID)`
プレイヤーを電線に吸着させます。

```cpp
void PlayerState_Human_MidAir::HandleInput(Player& player)
{
    // 変身ボタンが押され、変身可能な場合
    if (player.inputHandler->IsIssued<CommandType::MORPH>() && 
        player.morphSystem->CanMorph())
    {
        // 最寄りの電線IDを取得
        PowerLineID lineID = player.morphSystem->GetNearestPowerLineID();
        
        // 電線に吸着
        player.movement->SnapToPowerLine(lineID);
        
        // 電気状態に遷移
        player.stateMachine->ChangeState(&PlayerStates::Electric, player);
    }
}
```

#### `void LineMove()`
電線上を移動します。

```cpp
void PlayerState_Electric::Update(Player& player)
{
    // 電線上を移動
    player.movement->LineMove();
}
```

#### `void Turn(float inputX, float inputZ)`
電線上で方向転換を行います。

```cpp
void PlayerState_Electric::HandleInput(Player& player)
{
    float inputX = player.inputHandler->GetValue<CommandType::MOVE_HORIZONTAL>();
    float inputZ = player.inputHandler->GetValue<CommandType::MOVE_VERTICAL>();
    
    // 入力があれば方向転換
    if (inputX != 0.0f || inputZ != 0.0f)
    {
        player.movement->Turn(inputX, inputZ);
    }
}
```

---

### PlayerMoveCtx（移動設定）

移動に関する各種パラメータを設定できます。

```cpp
struct PlayerMoveCtx
{
    float WalkSpeed = 10.0f;           // 歩行速度
    float AirMoveSpeed = 3.0f;         // 空中移動速度
    float JumpForce = 20.0f;           // ジャンプ力
    float ElectricJumpForce = 20.0f;   // 電気ジャンプ力
    float LineMoveSpeed = 0.0f;        // 電線移動速度
    float Gravity = -30.0f;            // 重力加速度
    float GroundDetectOffset = 0.5f;   // 地面検出オフセット
    // ...
};

// 使用例
PlayerMoveCtx moveCtx;
moveCtx.WalkSpeed = 15.0f;
moveCtx.JumpForce = 25.0f;

auto* movement = new PlayerMovement(camera, moveCtx);
```

---

# `InputHandler` クラス

## 概要
入力デバイス（キーボード/ゲームパッド）からの入力を抽象化し、コマンドパターンで管理します。

## 使い方

### 入力の取得

#### `float GetValue<CommandType>()`
コマンドの値を取得します（0.0～1.0、または-1.0～1.0）。

```cpp
// 水平移動入力を取得
float horizontal = inputHandler->GetValue<CommandType::MOVE_HORIZONTAL>();

// 垂直移動入力を取得
float vertical = inputHandler->GetValue<CommandType::MOVE_VERTICAL>();

// ジャンプボタンの入力を取得
float jumpValue = inputHandler->GetValue<CommandType::JUMP>();
```

#### `bool IsIssued<CommandType>()`
コマンドが発行されているか（入力があるか）を取得します。

```cpp
// ジャンプボタンが押されているか
if (inputHandler->IsIssued<CommandType::JUMP>())
{
    player.movement->GroundJump(inputX, inputZ);
}

// ダッシュボタンが押されているか
if (inputHandler->IsIssued<CommandType::RUN>())
{
    player.movement->Run(inputX, inputZ);
}

// 変身ボタンが押されているか
if (inputHandler->IsIssued<CommandType::MORPH>())
{
    // 変身処理
}
```

---

# `PlayerMorphSystem` クラス

## 概要
プレイヤーの変身（人間形態⇔電気形態）を管理するシステムです。

## 主な機能

### `bool CanMorph()`
現在変身可能かどうかを判定します。

```cpp
void PlayerState_Human_MidAir::HandleInput(Player& player)
{
    // 変身ボタンが押された
    if (player.inputHandler->IsIssued<CommandType::MORPH>())
    {
        // 変身可能か判定
        if (player.morphSystem->CanMorph())
        {
            // 変身処理
            PowerLineID lineID = player.morphSystem->GetNearestPowerLineID();
            player.movement->SnapToPowerLine(lineID);
            
            // モデルを変更
            player.meshRenderer->SetModel(player.electricModel);
            
            // 電気状態に遷移
            player.stateMachine->ChangeState(&PlayerStates::Electric, player);
        }
    }
}
```

### `PowerLineID GetNearestPowerLineID()`
最も近い電線のIDを取得します。

```cpp
// 最寄りの電線IDを取得
PowerLineID nearestLine = player.morphSystem->GetNearestPowerLineID();

// 電線に吸着
player.movement->SnapToPowerLine(nearestLine);
```

---

# `TPSCamera` クラス

## 概要
プレイヤーを追従する三人称視点カメラです。マウスやゲームパッドのスティック入力でカメラを回転できます。

## 使い方

### カメラの生成
```cpp
// カメラオブジェクトを生成
GameObject* cameraObj = CreateGameObject();
cameraObj->SetTag("Camera");

// TPSCameraCtxで設定
TPSCameraCtx cameraCtx;
cameraCtx.Distance = 15.0f;       // プレイヤーからの距離
cameraCtx.Height = 3.0f;          // カメラの高さ
cameraCtx.SensitivityX = 0.5f;    // 水平回転感度
cameraCtx.SensitivityY = 0.5f;    // 垂直回転感度
cameraCtx.FollowSpeed = 3.0f;     // 追従速度

// TPSCameraコンポーネントを追加
auto* camera = cameraObj->AddComponent<TPSCamera>(playerObj, cameraCtx);

// Playerにカメラへの参照を設定
player->camera = camera;
```

### カメラの回転
```cpp
void PlayerState_Human_Idle::Update(Player& player)
{
    // カメラ回転入力を取得
    float cameraX = player.inputHandler->GetValue<CommandType::CAMERA_HORIZONTAL>();
    float cameraY = player.inputHandler->GetValue<CommandType::CAMERA_VERTICAL>();
    
    // カメラを回転
    player.camera->Rotate(cameraX, cameraY);
}
```

---

# `Health` クラス

## 概要
プレイヤーの体力を管理するコンポーネントです。ダメージや回復の処理を提供します。

## 使い方

### 体力の初期化
```cpp
// 最大体力100でHealthコンポーネントを追加
auto* health = playerObj->AddComponent<Health>(100.0f);

// Playerに参照を設定
player->health = health;
```

### ダメージと回復
```cpp
class DamageZone : public ScriptComponent
{
public:
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (!body) return;
        
        // トリガー内のプレイヤーを検出
        auto triggers = body->GetTriggerStay();
        for (auto* other : triggers)
        {
            GameObject& otherObj = other->gameObject();
            
            if (otherObj.CompareTag("Player"))
            {
                // プレイヤーにダメージ
                auto* health = otherObj.GetComponent<Health>();
                if (health)
                {
                    health->TakeDamage(10.0f * Time::DeltaTime());
                    
                    // 体力が0になったら
                    if (!health->IsAlive())
                    {
                        std::cout << "Player died!" << std::endl;
                        // ゲームオーバー処理など
                    }
                }
            }
        }
    }
};
```

### 回復アイテム
```cpp
class HealItem : public ScriptComponent
{
public:
    void Update() override
    {
        auto* body = gameObject().GetComponent<PhysicsBody>();
        if (!body) return;
        
        auto triggers = body->GetTriggerEnter();
        for (auto* other : triggers)
        {
            GameObject& otherObj = other->gameObject();
            
            if (otherObj.CompareTag("Player"))
            {
                // プレイヤーを回復
                auto* health = otherObj.GetComponent<Health>();
                if (health)
                {
                    health->Heal(30.0f);
                    std::cout << "Healed! Current HP: " << health->GetCurrentHealth() << std::endl;
                }
                
                // アイテムを破棄
                gameObject().Destroy();
            }
        }
    }
};
```

---

# プレイヤーシステム全体の実装例

## 完全なプレイヤー生成

```cpp
class GameScene : public Scene
{
public:
    void Initialize() override
    {
        // プレイヤーオブジェクトを生成
        GameObject* playerObj = CreateGameObject();
        playerObj->SetTag("Player");
        playerObj->transform().SetPosition(Vector3(0.0f, 5.0f, 0.0f));
        
        // Playerコンポーネントを追加
        auto* player = playerObj->AddComponent<Player>();
        
        // PhysicsBodyを追加
        PhysicsBodyDesc bodyDesc;
        bodyDesc.Type = BodyType::DYNAMIC;
        bodyDesc.Mass = 70.0f;
        bodyDesc.FixedRotation = Vector3(1.0f, 0.0f, 1.0f);
        auto* physicsBody = playerObj->AddComponent<PhysicsBody>(bodyDesc);
        player->physicsBody = physicsBody;
        
        // ColliderShapeを追加
        CapsuleColliderDesc capsuleDesc;
        capsuleDesc.Radius = 0.5f;
        capsuleDesc.Height = 1.8f;
        capsuleDesc.OffsetPos = Vector3(0.0f, 0.9f, 0.0f);
        playerObj->AddComponent<ColliderShape>(capsuleDesc);
        
        // カメラを生成
        GameObject* cameraObj = CreateGameObject();
        TPSCameraCtx cameraCtx;
        cameraCtx.Distance = 10.0f;
        cameraCtx.Height = 3.0f;
        auto* camera = cameraObj->AddComponent<TPSCamera>(playerObj, cameraCtx);
        player->camera = camera;
        
        // 体力コンポーネントを追加
        auto* health = playerObj->AddComponent<Health>(100.0f);
        player->health = health;
        
        // MeshRendererを追加
        auto* renderer = playerObj->AddComponent<MeshRenderer>();
        player->meshRenderer = renderer;
        
        // モデルを読み込み
        player->humanModel = resource().Load<MODEL>("models/player_human.obj");
        player->electricModel = resource().Load<MODEL>("models/player_electric.obj");
        renderer->SetModel(player->humanModel);
        
        std::cout << "Player initialized successfully!" << std::endl;
    }
};
```

---

## カスタムプレイヤーステートの作成

```cpp
// カスタム状態の定義
class PlayerState_Custom : public PlayerState
{
public:
    void Enter(Player& player) override
    {
        std::cout << "Entering custom state" << std::endl;
        // 状態開始時の処理
    }
    
    void HandleInput(Player& player) override
    {
        // ジャンプで別の状態に遷移
        if (player.inputHandler->IsIssued<CommandType::JUMP>())
        {
            player.stateMachine->ChangeState(&PlayerStates::HumanMidAir, player);
        }
    }
    
    void Update(Player& player) override
    {
        // 毎フレームの処理
        float inputX = player.inputHandler->GetValue<CommandType::MOVE_HORIZONTAL>();
        float inputZ = player.inputHandler->GetValue<CommandType::MOVE_VERTICAL>();
        
        // カスタム移動処理
        player.movement->Walk(inputX, inputZ);
    }
};

// 状態の使用
PlayerState_Custom customState;
player.stateMachine->ChangeState(&customState, player);
```

---

# 重要なポイント

## 1. コンポーネント間の連携
各コンポーネントは `Player` クラスを通じて連携します。`Player` が中央ハブとして機能し、各コンポーネントへの参照を保持します。

## 2. ステートパターンの活用
プレイヤーの振る舞いは状態ごとに分離されており、状態の追加・変更が容易です。

## 3. 入力の抽象化
`InputHandler` を使うことで、入力デバイスに依存しないコードが書けます。

## 4. 移動制御の分離
`PlayerMovement` に移動ロジックをまとめることで、状態クラスがシンプルになります。

## 5. カメラの独立性
`TPSCamera` は独立したコンポーネントとして実装され、プレイヤーとは疎結合です。

---

# まとめ

プレイヤーシステムは以下のように構成されています：

1. **`Player`** - 全体を統括する中核クラス
2. **`PlayerStateMachine`** - 状態管理
3. **`PlayerMovement`** - 移動制御
4. **`InputHandler`** - 入力処理
5. **`PlayerMorphSystem`** - 変身システム
6. **`TPSCamera`** - カメラ制御
7. **`Health`** - 体力管理

これらのコンポーネントを組み合わせることで、拡張性の高いプレイヤーシステムを構築できます。各コンポーネントは独立しており、個別にテストや変更が可能です。
