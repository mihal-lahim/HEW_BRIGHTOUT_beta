# Copilot Instructions

## General Guidelines
- First general instruction
- Second general instruction

## Code Style
- Use specific formatting rules
- Follow naming conventions

## Project-Specific Rules
- PhysicsSystem の登録/登録解除は外部から Add/Remove 関数経由で行う方針。
- Bullet オブジェクトの所有/解放は Component 側で行う方針。
- EngineCoreが各Systemを持ち、それの参照を詰め合わせたGameContextを提供する
- SystemはコンストラクタでEngineCoreの参照を受け取る
- SystemはEngineCoreを通じてGameContextを取得し、他のSystemにアクセスする
- SceneはGameObjectとComponentの管理を行う。
- ComponentにはSystem管理のComponentとユーザー定義のScriptComponentがある。
- System依存のComponentはSystemがSceneからGetComponentsし、更新を行う
- GameObjectはComponentを持ち、追加/削除/取得の関数を提供する
- PrefabはGameObjectのテンプレートとして機能し、インスタンス化が可能