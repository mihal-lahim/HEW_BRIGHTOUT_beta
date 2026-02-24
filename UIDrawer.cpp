#include "UIDrawer.h"
#include "RenderingSystem.h"
#include "UIQuad.h"
#include "SceneSystem.h" 

GameObject* UI::CreateUI(GameObject* owner, const std::wstring& texturePath, const Vector3& position, const Vector3& scale,
	const std::string& vsPath, const std::string& psPath)
{
	// Scene の CreateGameObject を呼び出す
	GameObject* uiObject = owner->scenePtr()->CreateGameObject();

	// MeshRenderer を追加
	MeshRenderer* renderer = uiObject->AddComponent<MeshRenderer>();

	// メッシュ設定（RenderingSystem の CreateUIQuad を利用）
	renderer->mesh = owner->rendering().CreateUIQuad().get();
	renderer->renderQueue = RenderQueue::UI;

	// マテリアル設定
	renderer->material.vsPath = vsPath;
	renderer->material.psPath = psPath;
	renderer->material.texturePath = texturePath;

	// Transform 設定
	uiObject->transform().position() = position;
	uiObject->transform().scale() = scale;

	return uiObject;
}
