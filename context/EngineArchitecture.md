# BRIGHTOUT ゲームエンジン アーキテクチャ概要

## 全体構成

本プロジェクトは DirectX 11 ベースの自作ゲームエンジンで構築されています。
エンジンは **System（サブシステム）群** がゲームの各機能を担当し、
**Scene → GameObject → Component** の階層でゲームロジックを構成します。

```
┌───────────────────────────────────────────────┐
│                   WinMain                     │
│  ┌──────────────┐  ┌───────────────────────┐  │
│  │   Window      │  │   GraphicsDevice      │  │
│  │ (HWND管理)    │  │ (D3D11 Device/Context)│  │
│  └──────┬───────┘  └───────────┬───────────┘  │
│         │                      │              │
│  ┌──────┴──────────────────────┴──────────┐   │
│  │              EngineCore                │   │
│  │  ┌────────────┐  ┌───────────────────┐ │   │
│  │  │WindowSystem│  │  RenderingSystem  │ │   │
│  │  ├────────────┤  ├───────────────────┤ │   │
│  │  │SceneSystem │  │ ResourceSystem    │ │   │
│  │  ├────────────┤  ├───────────────────┤ │   │
│  │  │PhysicsSystem│ │  InputSystem      │ │   │
│  │  └────────────┘  └───────────────────┘ │   │
│  └────────────────────────────────────────┘   │
└───────────────────────────────────────────────┘
```

---

## エントリーポイント (`main.cpp`)

```
WinMain
  ├─ CoInitializeEx          … COM初期化
  ├─ EngineCore 構築         … 全Systemを生成
  ├─ Window / GraphicsDevice … ウィンドウとD3D11の初期化
  ├─ RenderingSystem::Initialize
  ├─ SceneSystem::ChangeScene<Title>  … 初期シーン設定
  └─ ゲームループ
       ├─ PeekMessage          … Windowsメッセージ処理
       └─ EngineCore::Update   … 毎フレーム更新
```

`EngineCore::Update` の中では以下が順に実行されます。

```
EngineCore::Update()
  ├─ InputSystem::Update()          … 入力状態の更新
  ├─ SceneSystem::Update()          … シーンの更新（後述）
  └─ RenderingSystem::Render(scene) … 描画パイプライン実行
```

---

## EngineCore

**ファイル:** `include\Engine\EngineCore.h` / `src\Engine\EngineCore.cpp`

ゲーム全体を統括するシングルトン的クラスです。
6つのサブシステムを `std::unique_ptr` で所有し、ライフサイクルを管理します。

| メンバ | 型 | 役割 |
|--------|------|------|
| `m_windowSystem` | `WindowSystem` | ウィンドウ管理 |
| `m_sceneSystem` | `SceneSystem` | シーン遷移・管理 |
| `m_physicsSystem` | `PhysicsSystem` | 物理演算 |
| `m_inputSystem` | `InputSystem` | キーボード/マウス/ゲームパッド入力 |
| `m_renderingSystem` | `RenderingSystem` | 3D/UI描画 |
| `m_resourceSystem` | `ResourceSystem` | テクスチャ/シェーダー/モデル管理 |

各Systemへの横断的アクセスは `GameContext` 構造体を介して行います。

---

## GameContext

**ファイル:** `include\Engine\GameContext.h`

```cpp
struct GameContext
{
    WindowSystem*    windowSystem;
    SceneSystem*     sceneSystem;
    PhysicsSystem*   physicsSystem;
    InputSystem*     inputSystem;
    RenderingSystem* renderingSystem;
    ResourceSystem*  resourceSystem;
};
```

`EngineCore::GetGameContext()` で取得でき、`Scene` / `GameObject` / `Component` に配布されます。
これにより各階層から任意の System へアクセス可能です。

---

## SceneSystem（シーン管理）

**ファイル:** `include\Engine\SceneSystem.h` / `src\Engine\SceneSystem.cpp` / `include\Engine\SceneSystem.inl`

### 役割

- 現在アクティブなシーン (`m_currentScene`) の保持
- シーン遷移（Change / Push / Pop）
- 遷移の遅延実行（フレーム境界で安全に切り替え）

### シーン遷移の仕組み

```
ChangeScene<T>() が呼ばれる
  │
  ├─ m_pendingSceneChange にラムダを登録（遅延）
  │
  └─ SceneSystem::Update() のフレーム先頭/末尾で
     ProcessPendingSceneChange() が実行
       │
       └─ ChangeSceneImmediate<T>()
            ├─ 旧シーン: OnExit() → Finalize() → Disable()
            ├─ 新シーン: make_unique<T>() で生成
            └─ 新シーン: Enable() → OnEnter() → Initialize()
```

### 遷移メソッド一覧

| メソッド | 説明 |
|----------|------|
| `ChangeScene<T>()` | 現在のシーンを破棄して新シーンへ遷移 |
| `PushScene<T>()` | 現在のシーンをスタックに保存し、新シーンへ遷移 |
| `PopScene<T>()` | スタックから前のシーンを復帰 |

### Update フロー

```
SceneSystem::Update()
  ├─ ProcessPendingSceneChange()   … 遅延遷移の実行
  ├─ Scene::UpdateScene()          … 現在シーンの更新
  └─ ProcessPendingSceneChange()   … フレーム末の遷移実行
```

---

## Scene（シーン基底クラス）

**ファイル:** `include\Engine\Scene.h` / `src\Engine\Scene.cpp`

### 役割

- `GameObject` と `Component` の生成・管理・破棄
- コンポーネントの更新サイクル実行
- 各 System へのアクセサ提供

### ライフサイクル

```
Enable()          … コンポーネントの OnEnable 呼び出し
  ↓
OnEnter()         … シーン固有の開始処理（オーバーライド可能）
  ↓
Initialize()      … シーンの初期化（GameObject/Component 生成）
  ↓
  ┌─── UpdateScene() 毎フレーム ───┐
  │  Time::BeginFrame()             │
  │  PhysicsUpdate (固定ステップ)   │
  │  Cycle()                        │
  │    ├─ AddPending (GO/Comp)      │
  │    ├─ Start()                   │
  │    ├─ PreUpdate()               │
  │    ├─ Update()                  │
  │    ├─ PostUpdate()              │
  │    ├─ AddPending (遅延追加分)   │
  │    └─ DestroyPending            │
  │  Update(deltaTime)              │
  └─────────────────────────────────┘
  ↓
OnExit()          … シーン固有の終了処理（オーバーライド可能）
  ↓
Finalize()        … シーンの終了処理
  ↓
Disable()         … コンポーネントの OnDisable 呼び出し
```

### 主要メソッド

| メソッド | 説明 |
|----------|------|
| `CreateGameObject()` | 新しい `GameObject` を生成 |
| `Instantiate(Prefab&)` | `Prefab` から `GameObject` を生成 |
| `GetComponents<T>()` | シーン内の全 `T` コンポーネントを取得 |
| `DestroyGameObject(go)` | `GameObject` の遅延破棄を予約 |
| `sceneSystem()` | `SceneSystem` への参照取得 |
| `rendering()` | `RenderingSystem` への参照取得 |
| `input()` | `InputSystem` への参照取得 |
| `resource()` | `ResourceSystem` への参照取得 |
| `physics()` | `PhysicsSystem` への参照取得 |

### オーバーライド可能な仮想メソッド

| メソッド | 説明 |
|----------|------|
| `Initialize()` | シーン開始時の初期化処理 |
| `Finalize()` | シーン終了時の片付け処理 |
| `OnEnter()` | シーン遷移直後のフック |
| `OnExit()` | シーン遷移直前のフック |
| `Update(float deltaTime)` | 毎フレームのシーン固有更新 |
| `Draw()` | 描画パイプライン内での追加描画 |

---

## GameObject

**ファイル:** `include\Engine\GameObject.h`

### 役割

- `Component` のコンテナ
- `Transform` を必ず所持
- 親子関係の管理
- タグ・名前による識別

### 構成図

```
GameObject
  ├─ Transform*         (必須・自動付与)
  ├─ Component* 配列    (AddComponent で追加)
  ├─ 親子関係            (SetParent / SetChild)
  ├─ Tag / Name          (識別用文字列)
  └─ Active状態          (SetActive)
```

### 主要メソッド

| メソッド | 説明 |
|----------|------|
| `transform()` | Transform コンポーネント取得 |
| `AddComponent<T>(args...)` | コンポーネント追加 |
| `GetComponent<T>()` | 特定型のコンポーネント取得 |
| `SetActive(bool)` | アクティブ状態の設定 |
| `SetTag(string)` / `CompareTag(string)` | タグの設定・比較 |
| `SetParent(go)` / `SetChild(go)` | 親子関係の設定 |
| `Destroy()` | 自身の破棄を予約 |

---

## Component / ScriptComponent

**ファイル:** `include\Engine\Component.h` / `include\Engine\ScriptComponent.h`

### Component 階層

```
Object
  └─ Component           … 全コンポーネントの基底
       ├─ Transform       … 位置・回転・スケール
       ├─ MeshRenderer    … メッシュ描画
       ├─ Camera          … カメラ
       ├─ PhysicsBody     … 物理ボディ
       ├─ ColliderShape   … コライダー
       ├─ Animator        … アニメーション
       └─ ScriptComponent … ユーザー定義ロジック用基底
            ├─ FadeController
            ├─ TitleController
            ├─ ResultController
            ├─ Player
            ├─ Health
            ├─ Movement
            └─ ... (ゲームロジック各種)
```

### Component の分類

| 種類 | 管理方式 | 更新方法 |
|------|----------|----------|
| **System管理Component** | `Scene::m_components` プールで管理 | System が `GetComponents<T>()` で取得し更新 |
| **ScriptComponent** | `Scene::m_scriptComponents` プールで管理 | Scene の Cycle 内で自動的に `Start` / `Update` 等を呼び出し |

### ScriptComponent ライフサイクル

```
Awake()       … プールに登録された直後
  ↓
Start()       … 最初の Update 前に一度だけ
  ↓
PreUpdate()   … Update の前処理
  ↓
Update()      … メインの更新処理
  ↓
PostUpdate()  … Update の後処理
  ↓
OnDestroy()   … 破棄時
```

---

## ObjectPool（オブジェクトプール）

**ファイル:** `include\Engine\ObjectPool.h`

`Scene` 内の `GameObject` と `Component` はすべて `ObjectPool<T>` で管理されます。
各オブジェクトは `allocationID` で識別され、プール上のインデックスとして機能します。

```
Scene
  ├─ m_gameObjects          : ObjectPool<GameObject>
  ├─ m_components[]         : ObjectPool<Component>   × 型数
  └─ m_scriptComponents[]   : ObjectPool<ScriptComponent> × 型数
```

生成・破棄は **遅延キュー** 方式で安全に処理されます。

```
生成: CreateGameObject() → m_pendingAddGameObjects キュー → Cycle先頭で Register
破棄: Destroy()          → m_pendingDestroy キュー       → Cycle末尾で Destroy
```

---

## Prefab

**ファイル:** `include\Engine\Prefab.h`

`GameObject` のテンプレートとして機能する抽象クラスです。
`Instantiate(GameObject&)` をオーバーライドし、コンポーネントの追加やパラメータ設定を行います。

```cpp
class PlayerPrefab : public Prefab
{
public:
    void Instantiate(GameObject& gameObject) override
    {
        gameObject.AddComponent<Player>();
        gameObject.AddComponent<Movement>();
        gameObject.AddComponent<Health>();
    }
};

// 使用例
PlayerPrefab prefab;
GameObject* player = scene->Instantiate(prefab);
```

---

## RenderingSystem（描画システム）

**ファイル:** `include\Rendering\RenderingSystem.h` / `src\Rendering\RenderingSystem.cpp`

### 描画パイプライン

```
RenderingSystem::Render(scene)
  ├─ GraphicsDevice::Clear()
  ├─ UpdatePerFrame()        … ライティング定数バッファ (b0)
  │
  ├─ [アニメーション更新]
  │    Animator::UpdateAnimation() / Bind()
  │
  ├─ [3D描画] カメラごとにループ
  │    ├─ UpdatePerCamera()  … View/Projection 定数バッファ (b1)
  │    ├─ Background キュー描画
  │    ├─ Opaque / Transparent キュー描画
  │    └─ SkinnedMesh 描画
  │
  ├─ [UI描画]
  │    ├─ 正射影カメラ設定 (0,0)-(1920,1080)
  │    ├─ UIParams 定数バッファ (b4)
  │    └─ RenderQueue::UI のレンダラー描画
  │
  ├─ Scene::Draw()           … シーン固有の追加描画フック
  │
  └─ GraphicsDevice::Present()
```

### 定数バッファ構成

| スロット | 名称 | 内容 |
|----------|------|------|
| b0 | PerFrame | 環境光色、ディレクショナルライト色・方向 |
| b1 | PerCamera | View行列、Projection行列 |
| b2 | PerObject | World行列 |
| b4 | UIParams | 画面幅、画面高さ、UseScreenSpace |

### RenderQueue

| キュー | 深度テスト | アルファブレンド | カリング |
|--------|-----------|-----------------|---------|
| Background | OFF | 不透明 | NONE |
| Opaque | ON | 不透明 | BACK |
| Transparent | ON | 透過 | BACK |
| UI | OFF | 透過 | BACK |

---

## ResourceSystem（リソース管理）

**ファイル:** `include\Rendering\ResourceSystem.h` / `include\Rendering\ResourceSystem.inl`

テクスチャ・シェーダー・モデルなどのリソースを **参照カウント方式** で管理します。

```
ResourceSystem::Load<T>(args...)
  ├─ ハッシュキーでキャッシュ検索
  ├─ 既存 → 参照カウント++ して返却
  └─ 未生成 → new T → CreateBuffer() → キャッシュに登録
```

| リソース型 | キー生成方式 |
|-----------|-------------|
| `Texture` | `std::hash<wstring>(filePath)` |
| `ShaderProgram` | `hash(vsPath) ^ hash(psPath)` |
| `Model` | `std::hash<string>(filePath)` |
| `Material` | ユニークキー自動生成 |

---

## InputSystem（入力システム）

キーボード (`Keyboard`)、マウス (`Mouse`)、ゲームパッド (`GamePad`) を統合管理します。
各入力デバイスは `IsDown` / `IsPressed` / `IsReleased` メソッドで状態を取得できます。

---

## PhysicsSystem（物理システム）

Bullet Physics を使用した物理演算システムです。
固定タイムステップ (`Time::FixedDeltaTime` = 1/60秒) で物理更新を行います。

```
Scene::UpdateScene()
  └─ while (Time::HasFixedStep())
       └─ PhysicsSystem::PhysicsUpdate(scene, fixedDt)
```

---

## FadeController（フェード制御）

**ファイル:** `include\UI\FadeController.h` / `src\UI\FadeController.cpp`

シーン遷移時のフェードイン/アウトを制御する `ScriptComponent` です。

### 使用パターン

```cpp
// ScriptComponent 内で
m_fadeController = gameObject().AddComponent<FadeController>();

// フェードイン（画面が明るくなる）
m_fadeController->StartFadeIn(1.0f, [this]() {
    // フェード完了コールバック
});

// フェードアウト（画面が暗くなる）→ シーン遷移
m_fadeController->StartFadeOut(1.0f, [this]() {
    scene().ChangeScene<NextScene>();
});
```

### フェードの仕組み

- 全画面UIクアッド (`fade.png`) を `RenderQueue::UI` で描画
- `FadePS.cso` シェーダーでアルファ値を制御
- `steady_clock` ベースの独自デルタタイム計測で安定動作

---

## Time（時間管理）

**ファイル:** `include\Math\GameTime.h`

`steady_clock` ベースのフレーム時間管理クラスです。

| メソッド | 説明 |
|----------|------|
| `DeltaTime()` | 前フレームからの経過時間（秒） |
| `FixedDeltaTime()` | 固定更新間隔（デフォルト 1/60秒） |
| `TimeScale()` | 時間の倍率 |
| `BeginFrame()` | フレーム開始時に呼び出し |
| `HasFixedStep()` | 固定ステップが溜まっているか |
| `ConsumeFixedStep()` | 固定ステップを1回消費 |

---

## シーン一覧と遷移フロー

```
Title  ──(Enter/ボタン)──→  Namioka(Game)  ──(クリア条件)──→  Result
  ↑                                                              │
  └──────────────────(Enter/ボタン)───────────────────────────────┘
```

### 既存シーン

| シーン | ファイル | 説明 |
|--------|----------|------|
| `Title` | `include\Scenes\Title.h` | タイトル画面。BGM再生、UI表示 |
| `Namioka` (Game) | `include\Scenes\Namioka.h` | メインゲームシーン |
| `Game` | `include\Scenes\Game.h` | テスト/デバッグ用ゲームシーン |
| `Result` | `include\Scenes\Result.h` | リザルト画面 |

### シーン遷移パターン（TitleController の例）

```
TitleController::Start()
  └─ FadeController::StartFadeIn(1.0秒)
       └─ コールバック: m_fadeReady = true

TitleController::Update()
  └─ ボタン押下検知
       └─ FadeController::StartFadeOut(1.0秒)
            └─ コールバック: scene().ChangeScene<Namioka>()
```

---

## ディレクトリ構成

```
HEW_BRIGHTOUT_beta/
├── include/
│   ├── Audio/          … Audio.h, BgmSwitcher.h, PlayerAudio.h
│   ├── Engine/         … EngineCore.h, Scene.h, SceneSystem.h,
│   │                     GameObject.h, Component.h, ScriptComponent.h,
│   │                     Prefab.h, ObjectPool.h, GameContext.h,
│   │                     Window.h, WindowSystem.h, EventSystem.h
│   ├── Gameplay/       … Player.h, Movement.h, Health.h, Camera.h,
│   │                     TPSCamera.h, PowerPlant.h, Enemy.h, ...
│   ├── Input/          … Keyboard.h, Mouse.h, GamePad.h, Command.h
│   ├── Math/           … Vector2/3/4.h, Quaternion.h, Transform.h,
│   │                     GameTime.h, SystemTimer.h, Ray.h
│   ├── Misc/           … InputSystem.h, PhysicsSystem.h, ScoreData.h
│   ├── Physics/        … PhysicsBody.h, ColliderShape.h
│   ├── Rendering/      … GraphicsDevice.h, RenderingSystem.h,
│   │                     ResourceSystem.h, Shader.h, Mesh.h,
│   │                     Material.h, Texture.h, Model.h,
│   │                     ConstantBuffer.h, Renderer.h,
│   │                     Animation.h, Billboard.h, VideoPlayer.h
│   ├── Scenes/         … Title.h, Game.h, Result.h, Namioka.h,
│   │                     TitleController.h, ResultController.h
│   └── UI/             … FadeController.h, UIDrawer.h, UIQuad.h,
│                         HPBarUI.h, MorphUI.h, TimerUI.h
├── src/
│   ├── Audio/          … 対応する .cpp
│   ├── Engine/         … 対応する .cpp
│   ├── Gameplay/       … 対応する .cpp
│   ├── Input/          … 対応する .cpp
│   ├── Math/           … 対応する .cpp
│   ├── Misc/           … main.cpp, InputSystem.cpp, PhysicsSystem.cpp
│   ├── Physics/        … PhysicsBody.cpp
│   ├── Rendering/      … 対応する .cpp
│   ├── Scenes/         … 対応する .cpp
│   └── UI/             … 対応する .cpp
├── context/            … 設計ドキュメント (*.md)
├── *.hlsl              … シェーダーファイル群
└── BRIGHTOUT.vcxproj   … プロジェクト定義
```

---

## クラス関係図

```
EngineCore ──所有──┬── WindowSystem
                   ├── SceneSystem ──管理── Scene (現在1つ + スタック)
                   ├── PhysicsSystem
                   ├── InputSystem
                   ├── RenderingSystem
                   └── ResourceSystem

Scene ──管理──┬── ObjectPool<GameObject>
              ├── ObjectPool<Component>[]
              └── ObjectPool<ScriptComponent>[]

GameObject ──保持──┬── Transform* (必須)
                   └── Component* [] (任意追加)

Component (基底)
  ├── Transform, MeshRenderer, Camera, PhysicsBody, ...
  └── ScriptComponent (基底)
        ├── FadeController
        ├── TitleController
        ├── Player, Health, Movement, ...
        └── ...

Prefab (基底) ── Instantiate → GameObject にコンポーネント追加
  ├── PlayerPrefab
  ├── EnemyPrefab
  ├── EnemySpawnerPrefab
  └── ModelPrefab
```

---

## 設計原則まとめ

| 原則 | 説明 |
|------|------|
| **System はコンストラクタで EngineCore 参照を受け取る** | `EngineCore` を介して `GameContext` を取得し、他 System にアクセス |
| **GameContext が横断的参照を提供** | Scene / GameObject / Component はすべて GameContext 経由でSystemにアクセス |
| **Scene が GameObject と Component の管理を行う** | 生成・破棄は遅延キュー方式で安全に処理 |
| **Component に System管理型とユーザー定義型がある** | System依存 Component は `GetComponents` で一括取得・更新。ScriptComponent は Scene が自動更新 |
| **PhysicsSystem の登録/登録解除は外部から `Add`/`Remove` 関数呼出で行う** | Component 側で Bullet オブジェクトの所有/生成を行う |
| **Prefab は GameObject のテンプレートとして機能** | `Instantiate` でインスタンス化可能 |
| **ConstantBuffer に行列を登録する際は必ず転置してから書き込む** | DirectX の列優先 → 行優先変換のため |
| **GameObject 名はモデル読み込み時の名前をそのまま使い、修飾しない** | 命名規則の統一 |
