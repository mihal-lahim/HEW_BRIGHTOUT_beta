
#ifndef COMPONENT_INL
#define COMPONENT_INL

#include "Component.h"


template<typename T>
	requires std::is_base_of<Component, T>::value
size_t Component::GetTypeID()
{
	static size_t id = m_counter++;
	return id;
}


#endif
