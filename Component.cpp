#include "Component.h"
#include "Scene.h"
#include "GameContext.h"
#include "GameObject.h"
#include "SceneSystem.h"

void Component::SetEnable(bool enable)
{
	if (m_isEnable == enable) return;
	m_isEnable = enable;
	if (m_isEnable)
	{
		OnEnable();
	}
	else
	{
		OnDisable();
	}
}

void Component::Destroy()
{
	if (!m_gameObject) return;

	// 自身を所有しているシーンに破壊を依頼
	m_gameObject->m_gameContext.sceneSystem->currentScene().DestroyComponent(this);
}

GameObject* Component::CreateGameObject()
{
	return m_gameObject->m_gameContext.sceneSystem->currentScene().CreateGameObject();
}

GameObject* Component::Instantiate(const Prefab& prefab)
{
	return m_gameObject->m_gameContext.sceneSystem->currentScene().Instantiate(prefab);
}

PhysicsSystem& Component::physics()
{
	return *m_gameObject->m_gameContext.physicsSystem;
}

SceneSystem& Component::scene()
{
	return *m_gameObject->m_gameContext.sceneSystem;
}

InputSystem& Component::input()
{
	return *m_gameObject->m_gameContext.inputSystem;
}
