#include "GameObject.h"
#include "GameContext.h"
#include "Scene.h"

void GameObject::SetActive(bool active)
{
	for (auto* comp : m_components)
	{
		comp->SetEnable(active);
	}
	m_isActive = active;
}

void GameObject::Destroy()
{
	if (!m_scene || m_isDestroyed) return;

	// 自身を所有しているシーンに破壊を依頼
	m_scene->DestroyGameObject(this);
	
	// 所有しているコンポーネントもすべて破壊を許可
	for (auto& comp : m_components)
		comp->Destroy();
}

GameObject* GameObject::CreateGameObject()
{
	return m_scene->CreateGameObject();
}

SceneSystem& GameObject::scene() const
{
	return m_scene->sceneSystem();
}

WindowSystem& GameObject::window() const
{
	return m_scene->window();
}

PhysicsSystem& GameObject::physics() const
{
	return m_scene->physics();
}

InputSystem& GameObject::input() const
{
	return m_scene->input();
}

RenderingSystem& GameObject::rendering() const
{
	return m_scene->rendering();
}

ResourceSystem& GameObject::resource() const
{
	return m_scene->resource();
}
