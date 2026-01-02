

#ifndef COMMAND_SET_H
#define COMMAND_SET_H


#include "command.h"
#include <unordered_set>


// コマンド型の概念
template<typename C>
concept CommandType = std::is_base_of_v<Command, C>;

class CommandSet
{
protected:
	// 許可されたコマンドIDの集合
	std::unordered_set<CommandID> m_AllowedCommands{};
public:
	CommandSet() = default;
	virtual ~CommandSet() = default;

	// コマンドを許可
	template<CommandType COM>
	void AllowCommand() { m_AllowedCommands.insert(CommandID(typeid(COM))); }

	// コマンドが許可されているか確認
	template<CommandType COM>
	bool IsCommandAllowed() const { return m_AllowedCommands.find(CommandID(typeid(COM))) != m_AllowedCommands.end(); }

};


#endif
