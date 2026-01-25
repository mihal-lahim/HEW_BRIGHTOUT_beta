#ifndef COMPONENT_H
#define COMPONENT_H

#include <cstdint>
#include <type_traits>
#include "Object.h"

class ObjectManager;
class GameObject;

class Component : public Object
{
private:
	// 所有しているゲームオブジェクト
	GameObject* m_GameObject = nullptr;

public:
	Component() = default;
	virtual ~Component() = default;

	// 所有しているゲームオブジェクトの取得メソッド
	GameObject* const GameObject() const { return m_GameObject; }

	// オブジェクトのアクティブ状態を取得するメソッド
	bool IsActive() const override;


	friend class ObjectManager;
};

// Component を継承している型に制約をかけるコンセプト
template<typename T>
concept ComponentDerived = std::is_base_of<Component, T>::value;


#endif
