#include "GameObject.h"
#include "GameContext.h"
#include "Scene.h"

void GameObject::Destroy()
{
	if (!m_scene) return;

	// 自身を所有しているシーンに破壊を依頼
	m_scene->DestroyGameObject(this);
	
	// 所有しているコンポーネントもすべて破壊を許可
	for (auto& comp : m_components)
		comp->Destroy();
}
