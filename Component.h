#ifndef COMPONENT_H
#define COMPONENT_H


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
	GameObject* const gameObject() const { return m_GameObject; }

	// オブジェクトのアクティブ状態を取得するメソッド
	bool IsActive() const override;

	friend class GameObject;
	friend class ObjectManager;
};


#endif
