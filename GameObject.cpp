#include "GameObject.h"
#include "ObjectManager.h"


GameObject::GameObject()
{
	objectManager()->RegisterGameObject(this);
}

void GameObject::RegisterComponent(Component* comp)
{
	comp->m_GameObject = this;
	objectManager()->RegisterComponent(comp);
}

void GameObject::SetActive(bool active)
{
	// 所持しているすべてのコンポーネントのアクティブ状態を設定
	auto comps = objectManager()->GetAllComponents(*this);

	// コンポーネントのアクティブ状態を設定
	for (auto comp : comps)
		comp->SetActive(active);


	Object::SetActive(active);
}
