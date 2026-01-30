
#ifndef COMPONENT_INL
#define COMPONENT_INL

#include "Component.h"
#include "GameObject.h"


template<typename T>
	requires std::is_base_of<Component, T>::value
uint32_t Component::GetTypeID()
{
	static uint32_t id = m_counter++;
	return id;
}

template<typename T, typename... Args>
T* Component::Instantiate(Args... args)
{
	// 所有しているゲームオブジェクトを通じて、新しいゲームオブジェクトを作成する
	return m_gameObject->Instantiate<T>(args...);
}


#endif
