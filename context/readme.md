# クラスの説明

## Scene

-GameObjectとComponentの管理を担当する

```cpp
virtual void Scene::Initialize() {}
```

-この関数をオーバーライドして各Sceneの初期化処理を作成。

## Gameの例

```cpp
virtual void Game::Initialize() override
{
    //ここにGameObjectの追加、Prefabの実体化などを書く
}
```

## GameObject

-基本的にはComponentを保持するクラス。
-必ずTransformを一つ持っている。
-文字列でタグを持ち、比較することができる。

```cpp

// Transformの取得
Transform& transform();

// タグを比較
bool CompareTag(std::string);

// Componentを追加するテンプレートメソッド
T* AddComponent();

// Componentを取得するテンプレートメソッド
T* GetComponent();

// Componentを全て取得するテンプレートメソッド
std::vector<T*> GetComponents();

//使い方
ColliderShape newShape = player.AddComponent<ColliderShape>();
ColliderShape* shape = player.GetComponent<ColliderShape>();

// アクティブ状態の設定、変更(持っているComponentの有効化無効化も変わる)
void SetActive();
bool IsActiveSelf();

// GameObjectを削除(持っているComponentも削除される)
void Destroy();

// 空のGameObjectを作成
GameObject* CreateGameObject();

// Prefabを実体化
GameObject* Instantiate(Prefab&);
```

## Component

-GameObjectの挙動を制御するクラス。

```cpp

// 親GameObjectの取得
GameObject* gameObject();


