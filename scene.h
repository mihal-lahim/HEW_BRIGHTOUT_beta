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
#include "RenderSystem.h"

class GameObject;

class Scene
{
protected:
	// オブジェクト管理システム
	ObjectManager m_ObjectManager{};

	// 物理演算システム
	PhysicsSystem m_PhysicsSystem{};

	// レンダリングシステム
	RenderSystem m_RenderSystem{};

public:

	Scene();
	virtual ~Scene();

	virtual void Initialize() {};
	void Update();
	virtual void Finalize() {};

	// オブジェクト管理システム取得メソッド
	ObjectManager& GetObjectManager() { return m_ObjectManager; }

	// 物理演算システム取得メソッド
	PhysicsSystem& GetPhysicsSystem() { return m_PhysicsSystem; }

	// レンダリングシステム取得メソッド
	RenderSystem& GetRenderSystem() { return m_RenderSystem; }
};


#endif
