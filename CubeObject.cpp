#include "CubeObject.h"
#include "MeshRenderer.h"
#include "Collider.h"

void CubeObject::Awake()
{
	// MeshRendererコンポーネント追加
	AddComponent<MeshRenderer>();

	// Colliderコンポーネント追加
	Collider* collider = new Collider(ColliderType::BOX, { 1.0f,1.0f,1.0f });
	RegisterComponent(collider);
}
