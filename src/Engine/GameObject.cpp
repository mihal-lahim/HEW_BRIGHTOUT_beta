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

bool GameObject::IsActiveInHierarchy() const
{
	if (!m_isActive)
	{
		return false;
	}
	GameObject* parent = GetParent();
	if (parent)
	{
		return parent->IsActiveInHierarchy();
	}
	return true;
}

void GameObject::SetParent(GameObject& parent)
{
	m_transform->SetParent(parent.transform());
}

void GameObject::SetChild(GameObject& child)
{
	child.m_transform->SetParent(*m_transform);
}

GameObject* GameObject::GetParent() const
{
	if (m_transform->GetParent())
	{
		return &m_transform->GetParent()->gameObject();
	}
	return nullptr;
}

std::vector<GameObject*> GameObject::GetChildren() const
{
	std::vector<GameObject*> children;
	for (auto* childTransform : m_transform->GetChildren())
	{
		children.push_back(&childTransform->gameObject());
	}
	return children;
}

void GameObject::RemoveParent()
{
	m_transform->RemoveParent();
}

void GameObject::RemoveChild(GameObject& child)
{
	child.m_transform->RemoveParent();
}

GameObject* GameObject::FindChildByName(const std::string& name) const
{
	GameObject* result = nullptr;

	for (auto* child : GetChildren())
	{
		if (child->GetName() == name)
		{
			return child;
		}
		result = child->FindChildByName(name);

		if (result)
			break;
	}

	return result;
}

void GameObject::Destroy()
{
	if (!m_scene || m_isDestroyed) return;
	m_isDestroyed = true;

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

GameObject* GameObject::Instantiate(Prefab& prefab)
{
	return m_scene->Instantiate(prefab);
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
