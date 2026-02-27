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
- ユーザーは『画像ファイル表示（ビルボード演出）を削除しない』ことを希望している。- Prefab��GameObject�̃e���v���[�g�Ƃ��ċ@�\���A�C���X�^���X�����\�B