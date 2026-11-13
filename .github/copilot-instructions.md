# Copilot Instructions

## General Guidelines
- First general instruction
- Second general instruction
- Always invoke tools to gather necessary context (e.g., `get_file`/`get_currentfile`/`code_search`) before answering. コード変更前に必要な文脈をツールで取得し、Markdownでは`file`/`directory`/`function`/`class`をバッククォートで囲み、Visual Studioの設定/コマンド名は__二重下線__で囲む。
- When generating code blocks, use fenced blocks with the language and target file path (e.g., ```cpp path/to/file.cpp). コードブロックは言語とターゲットファイルパスを付ける。Use mermaid for diagrams and follow special escaping rules。
- Responses should be provided in Japanese (ja-JP) and use backticks for identifiers。応答は日本語。
- 明示的なログ出力をデバッグ時に追加することを推奨する。ユーザーはデバッグ時にログ出力を明示的に追加することを求める傾向がある。

## Code Style
- Use specific formatting rules
- Follow naming conventions
- 各`ConstantBuffer`に行列を登録する際は必ず転置（transpose）してから書き込む。

## Project-Specific Rules
- PhysicsSystem �̓o�^/�o�^�����͊O������ `Add`/`Remove` �֐��o�R�ōs�����j�B
- Bullet �I�u�W�F�N�g�̏��L/����� Component ���ōs�����j�B
- EngineCore���eSystem�������A����̎Q�Ƃ��l�ߍ��キング��GameContext��񋟂���B
- System�̓R���X�g���N�^��EngineCore�̎Q�Ƃ��󂯎��B
- System��EngineCore��ʂ���GameContext���擾���A����System�ɃA�N�Z�X����B
- Scene��GameObject��Component�̊Ǘ����s���B
- GameObject名はモデル読み込み時の名前をそのまま使い、修飾しない。
- Component�ɂ�System�Ǘ���Component�ƃ��[�U�[��`��ScriptComponent������B
- System�ˑ���Component��System��Scene����`GetComponents`���A�X�V���s���B
- GameObject��Component�������A�ǉ�/�폜/�擾�̊֐���񋟂���B
- Prefab��GameObject�̃e���v���[�g�Ƃ��ċ@�\���A�C���X�^���X�����\�B
- ユーザーは『画像ファイル表示（ビルボード演出）を削除しない』ことを希望している。
- 敵の攻撃エフェクトは敵の目の前かつ上向き（面が上向き）のビルボードとして表示する。攻撃エフェクトは半円の上側が上を向いたまま、敵から見てプレイヤー方向を向く位置と回転にする。攻撃エフェクトは固定オフセットではなく、敵から見たプレイヤー方向へ動的に配置する。攻撃エフェクトの回転も、位置と同様にプレイヤー方向へ動的に再設定する。攻撃エフェクトでは全体ビルボードを変更せず、個別オブジェクト側で回転が効く実装を優先する。攻撃エフェクトの向きはプレイヤー方向ではなく、カメラ向き（ビルボード）を優先する。ビルボード全体に影響する変更は避け、攻撃エフェクトだけを個別に調整する。攻撃エフェクトは一旦、専用シェーダーではなく通常のビルボード設定で表示する。