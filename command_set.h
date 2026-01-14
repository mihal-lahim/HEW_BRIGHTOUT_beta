



#ifndef COMMAND_SET_H
#define COMMAND_SET_H


#include "command.h"
#include "input_device.h"
#include <unordered_map>
#include "Component.h"


// コマンド型の概念
template<typename C>
concept CommandType = std::is_base_of_v<Command, C>;

// Enum型の概念
template <typename E>
concept EnumType = std::is_enum_v<E>;

class CommandSet : public Component
{
protected:
	// コマンド型から入力情報へのマップ
	std::unordered_map<CommandID, std::pair<InputKey, InputCondition>> m_CommandMap;
public:
	CommandSet() = default;
	virtual ~CommandSet() = default;

	// 入力とコマンドの登録
	template<EnumType INP, CommandType COM>
	void RegisterCommand(INP input, InputCondition inpCondition);

	// 登録済みのコマンドかどうかを確認
	template<CommandType COM>
	bool IsRegistered() const { return m_CommandMap.find(CommandID(typeid(COM))) != m_CommandMap.end(); }

	// コマンドの入力情報を取得
	template<CommandType COM>
	const std::pair<InputKey, InputCondition>* GetCommandInfo() const;
};


template<EnumType INP, CommandType COM>
inline void CommandSet::RegisterCommand(INP input, InputCondition inpCondition)
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
inline const std::pair<InputKey, InputCondition>* CommandSet::GetCommandInfo() const
{
	// コマンドのtype_indexを取得
	auto typeIndex = CommandID(typeid(COM));

	// マップから検索
	auto it = m_CommandMap.find(typeIndex);

	// 登録されていれば情報を返す
	if (it != m_CommandMap.end())
		return &(it->second);

	// 未登録の場合はnullptrを返す
	return nullptr;
}


#endif
