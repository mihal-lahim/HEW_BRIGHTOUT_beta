
#ifndef COMPONENT_INL
#define COMPONENT_INL

#include "Component.h"

template<typename T>
	requires std::is_base_of<Component, T>::value
uint32_t Component::GetTypeID()
{
	static uint32_t id = m_counter++;
	return id;
}

#endif
