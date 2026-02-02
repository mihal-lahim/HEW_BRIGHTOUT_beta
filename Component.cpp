#include "Scene.h"
#include "GameContext.h"
#include "GameObject.h"
#include "SceneSystem.h"
#include "Component.h"

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
	m_gameObject->scene().DestroyComponent(this);
}

GameObject* Component::CreateGameObject()
{
	return m_gameObject->scene().CreateGameObject();
}

GameObject* Component::Instantiate(Prefab& prefab)
{
	return m_gameObject->scene().Instantiate(prefab);
}

GameObject* Component::GetGameObjectByTag(const std::string& tag)
{
	return m_gameObject->scene().GetGameObjectByTag(tag);
}

std::vector<GameObject*> Component::GetGameObjectsByTag(const std::string& tag)
{
	return m_gameObject->scene().GetGameObjectsByTag(tag);
}

WindowSystem& Component::window()
{
	return m_gameObject->window();
}

PhysicsSystem& Component::physics()
{
	return m_gameObject->physics();
}

SceneSystem& Component::scene()
{
	return m_gameObject->sceneSystem();
}

InputSystem& Component::input()
{
	return m_gameObject->input();
}

ResourceSystem& Component::resource()
{
	return m_gameObject->resource();
}
