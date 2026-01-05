#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H


#include <unordered_map>
#include "input_device.h"
#include "command_set.h"
#include "GameObject.h"

// Enum型の概念
template <typename E>
concept EnumType = std::is_enum_v<E>;


class InputSystem : public GameObject
{
private:
	// 入力デバイス
	InputDevice* m_InputDevice = nullptr;

	// コマンド型から入力情報へのマップ
	std::unordered_map<CommandID, std::pair<InputKey, InputCondition>> m_CommandMap;

	// コマンドセット
	CommandSet* m_CommandSet = nullptr;
public:
	// コンストラクタ・デストラクタ
	InputSystem(InputDevice* inputDevice, CommandSet* commandSet = nullptr)
		: m_InputDevice(inputDevice), m_CommandSet(commandSet)
	{}
	~InputSystem() = default;

	// デバイス設定
	void SetDevice(InputDevice* inputDevice) { m_InputDevice = inputDevice; }

	// 入力デバイスの設定
	void SetInputDevice(InputDevice* inputDevice) { m_InputDevice = inputDevice; }

	// 入力とコマンドの登録
	template<EnumType INP, CommandType COM>
	void RegisterCommand(INP input, InputCondition inpCondition);

	// 登録済みのコマンドかどうかを確認
	template<CommandType COM>
	bool IsRegistered() const { return m_CommandMap.find(CommandID(typeid(COM))) != m_CommandMap.end(); }


	// コマンドセットの設定
	void SetCommandSet(CommandSet* commandSet) { m_CommandSet = commandSet; }


	// コマンドの数値を取得
	template<CommandType COM>
	float GetValue() const;

	// コマンドが発行されているかどうかを取得
	template<CommandType COM>
	bool IsIssued() const { return GetCommandValue<COM>() != 0.0f; }

	// コマンドが許可されているかどうかを取得
	template<CommandType COM>
	bool IsAllowed() const { return m_CommandSet ? m_CommandSet->IsCommandAllowed<COM>() : false; }
};


template<EnumType INP, CommandType COM>
inline void InputSystem::RegisterCommand(INP input, InputCondition inpCondition)
{
	// WORD型にキャスト
	InputKey key = static_cast<InputKey>(input);

	// コマンドのtype_indexを取得
	auto typeIndex = CommandID(typeid(COM));

	// 既存のコマンドを検索
	auto it = m_CommandMap.find(typeIndex);

	// 既に登録されている場合は何もしない
	if (it != m_CommandMap.end()) return;

	// 入力コマンドリストに登録
	m_CommandMap[typeIndex] = { key, inpCondition };
}

template<CommandType COM>
inline float InputSystem::GetValue() const
{
	// コマンドのtype_indexを取得
	auto typeIndex = CommandID(typeid(COM));

	// マップから検索
	auto it = m_CommandMap.find(typeIndex);

	// 登録されていてかつ、許可されていれば値を返す
	if (it != m_CommandMap.end() && IsCommandAllowed<COM>())
		return m_InputDevice->GetInputValue(it->second.first, it->second.second);


	// 未登録または不許可の場合は0を返す
	return 0.0f;
}

#endif
