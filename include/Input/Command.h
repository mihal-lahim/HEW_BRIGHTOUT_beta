

#ifndef COMMAND_H
#define COMMAND_H

#include <typeindex>

typedef std::type_index CommandID;

class Command
{
public:
	Command() = delete;
	virtual ~Command() = default;
};


#endif
