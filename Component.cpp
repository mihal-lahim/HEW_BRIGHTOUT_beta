#include "Component.h"
#include "Scene.h"
#include "GameContext.h"
#include "GameObject.h"

bool Component::IsActive() const
{
	return m_gameObject->IsActiveInHierarchy() && m_isActive;
}

void Component::SetActive(bool active)
{
}

void Component::Destroy()
{
	if (!m_gameObject || !m_gameContext) return;

	// 自身を所有しているシーンに破壊を依頼
	m_gameContext->currentScene->DestroyComponent(this);
}
