# `GameObject` クラスの使い方

## 概要
`GameObject` は `Component` を保持する基本単位です。ゲーム内の実体を表現し、様々な `Component` を追加・削除・取得することで、オブジェクトに機能を持たせることができます。

---

## 基本的な関数

### 1. `Transform& transform()`
`GameObject` に必ず存在する `Transform` コンポーネントを取得します。

**使い方:**
```cpp
GameObject* obj = scene.CreateGameObject();
obj->transform().SetPosition(Vector3(0.0f, 5.0f, 0.0f));
obj->transform().SetRotation(Quaternion::Identity());
obj->transform().SetScale(Vector3(1.0f, 1.0f, 1.0f));
```

---

### 2. `void SetActive(bool active)`
`GameObject` のアクティブ状態を設定します。`false` にすると、オブジェクトと所属する全ての `Component` が無効化されます。

**使い方:**
```cpp
GameObject* obj = scene.CreateGameObject();
obj->SetActive(false); // オブジェクトを無効化
obj->SetActive(true);  // オブジェクトを有効化
```

---

### 3. `bool IsActiveSelf()`
自身のアクティブ状態を取得します。

**使い方:**
```cpp
if (obj->IsActiveSelf()) {
    // オブジェクトが有効な場合の処理
}
```

---

### 4. `bool IsActiveInHierarchy()`
階層を考慮したアクティブ状態を取得します（現在は `IsActiveSelf()` と同じ動作）。

**使い方:**
```cpp
if (obj->IsActiveInHierarchy()) {
    // 階層的にアクティブな場合の処理
}
```

---

### 5. `void Destroy()`
`GameObject` を破棄します。所属する全ての `Component` も同時に破棄されます。

**使い方:**
```cpp
GameObject* obj = scene.CreateGameObject();
// ... オブジェクトを使った処理
obj->Destroy(); // オブジェクトを破棄
```

---

### 6. `void SetTag(const std::string& tag)` / `bool CompareTag(const std::string& tag)`
タグの設定と比較を行います。タグはオブジェクトの分類に使用します。

**使い方:**
```cpp
GameObject* player = scene.CreateGameObject();
player->SetTag("Player");

if (player->CompareTag("Player")) {
    // プレイヤーオブジェクトの場合の処理
}
```

---

### 7. `GameObject* CreateGameObject()`
新しい `GameObject` を生成します。

**使い方:**
```cpp
GameObject* parent = scene.CreateGameObject();
GameObject* child = parent->CreateGameObject(); // 新しいオブジェクトを生成
```

---

## `Component` 関連の関数

### 8. `T* AddComponent<T>(Args&&... args)`
指定した型の `Component` を `GameObject` に追加します。

**使い方:**
```cpp
GameObject* obj = scene.CreateGameObject();

// MeshRendererコンポーネントを追加
auto* renderer = obj->AddComponent<MeshRenderer>();

// PhysicsBodyコンポーネントを引数付きで追加
auto* body = obj->AddComponent<PhysicsBody>(1.0f, 0.5f);
```

**注意:** `Transform` は `AddComponent` で追加できません（自動的に存在します）。

---

### 9. `T* GetComponent<T>()`
指定した型の `Component` を1つ取得します。存在しない場合は `nullptr` を返します。

**使い方:**
```cpp
GameObject* obj = scene.CreateGameObject();
obj->AddComponent<MeshRenderer>();

// MeshRendererコンポーネントを取得
auto* renderer = obj->GetComponent<MeshRenderer>();
if (renderer != nullptr) {
    // レンダラーが存在する場合の処理
}
```

---

### 10. `std::vector<T*> GetComponents<T>()`
指定した型の `Component` を全て取得します。複数の同じ型のコンポーネントがある場合に便利です。

**使い方:**
```cpp
GameObject* obj = scene.CreateGameObject();
obj->AddComponent<ScriptComponent>();
obj->AddComponent<ScriptComponent>();

// 全てのScriptComponentを取得
auto scripts = obj->GetComponents<ScriptComponent>();
for (auto* script : scripts) {
    // 各スクリプトに対する処理
}
```

---

## `System` アクセス関数

`GameObject` から各種システムにアクセスできます。

### 11. システムアクセス関数
- `SceneSystem& scene()`
- `WindowSystem& window()`
- `PhysicsSystem& physics()`
- `InputSystem& input()`
- `RenderingSystem& rendering()`
- `ResourceSystem& resource()`

**使い方:**
```cpp
GameObject* obj = scene.CreateGameObject();

// 入力システムにアクセス
if (obj->input().GetKey(KEY_SPACE)) {
    // スペースキーが押された場合の処理
}

// リソースシステムにアクセス
auto* mesh = obj->resource().Load<Mesh>("cube.obj");
```

---

## 使用例

### 基本的な使い方
```cpp
// GameObjectを生成
GameObject* player = scene.CreateGameObject();

// タグを設定
player->SetTag("Player");

// 位置を設定
player->transform().SetPosition(Vector3(0.0f, 0.0f, 0.0f));

// コンポーネントを追加
auto* renderer = player->AddComponent<MeshRenderer>();
auto* body = player->AddComponent<PhysicsBody>();

// コンポーネントを取得して使用
auto* rb = player->GetComponent<PhysicsBody>();
if (rb) {
    rb->AddForce(Vector3(0.0f, 10.0f, 0.0f));
}
```

### オブジェクトの無効化と破棄
```cpp
GameObject* enemy = scene.CreateGameObject();
enemy->SetTag("Enemy");

// 一時的に無効化
enemy->SetActive(false);

// 再度有効化
enemy->SetActive(true);

// オブジェクトを完全に破棄
enemy->Destroy();
```

---

## まとめ
`GameObject` は `Component` を管理するコンテナとして機能し、ゲーム内のあらゆるオブジェクトの基盤となります。`AddComponent` / `GetComponent` / `GetComponents` を使って柔軟に機能を追加・取得し、`SetActive` / `Destroy` でライフサイクルを管理します。
