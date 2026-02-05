# Copilot Instructions

## General Guidelines
- First general instruction
- Second general instruction
- Always invoke tools to gather necessary context (e.g., `get_file`/`get_currentfile`/`code_search`) before answering.
- When using markdown in assistant messages, format file, directory, function, and class names with backticks and wrap Visual Studio setting/command names with double underscores (e.g., `__setting_name__`).
- When generating code blocks, use fenced blocks with the language and target file path (e.g., ```cpp path/to/file.cpp). Use mermaid for diagrams and follow special escaping rules.
- Responses should be provided in Japanese (ja-JP) and use backticks for identifiers.

## Code Style
- Use specific formatting rules
- Follow naming conventions

## Project-Specific Rules
- PhysicsSystem �̓o�^/�o�^�����͊O������ `Add`/`Remove` �֐��o�R�ōs�����j�B
- Bullet �I�u�W�F�N�g�̏��L/����� Component ���ōs�����j�B
- EngineCore���eSystem�������A����̎Q�Ƃ��l�ߍ��킹��GameContext��񋟂���B
- System�̓R���X�g���N�^��EngineCore�̎Q�Ƃ��󂯎��B
- System��EngineCore��ʂ���GameContext���擾���A����System�ɃA�N�Z�X����B
- Scene��GameObject��Component�̊Ǘ����s���B
- Component�ɂ�System�Ǘ���Component�ƃ��[�U�[��`��ScriptComponent������B
- System�ˑ���Component��System��Scene����`GetComponents`���A�X�V���s���B
- GameObject��Component�������A�ǉ�/�폜/�擾�̊֐���񋟂���B
- Prefab��GameObject�̃e���v���[�g�Ƃ��ċ@�\���A�C���X�^���X�����\�B