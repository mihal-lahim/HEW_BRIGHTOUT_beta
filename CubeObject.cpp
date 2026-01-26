#include "CubeObject.h"
#include "MeshRenderer.h"

void CubeObject::Awake()
{
	// MeshRendererコンポーネント追加
	AddComponent<MeshRenderer>();

	// Transform初期化
	transform.Position = { 0.0f,0.0f,0.0f };
	transform.Rotation = Quaternion::Identity();
	transform.Scale = { 1.0f,1.0f,1.0f };
}
