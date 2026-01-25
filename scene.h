/*==============================================================================

   シーン管理[scene.h]
														 Author : syota
														 Date   : 2025/12/09
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SCENE_H
#define SCENE_H

#include "ObjectManager.h"
#include "PhysicsSystem.h"

class Scene
{
protected:
	// オブジェクト管理システム
	ObjectManager m_ObjectManager{};

	// 物理演算システム
	PhysicsSystem m_PhysicsSystem{};

public:
	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Exit() = 0;

	// オブジェクト管理システム取得メソッド
	ObjectManager& GetObjectManager() { return m_ObjectManager; }

	// 物理演算システム取得メソッド
	PhysicsSystem& GetPhysicsSystem() { return m_PhysicsSystem; }

};

#endif
