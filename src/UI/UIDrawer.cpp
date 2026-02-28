#include "UIDrawer.h"
#include "RenderingSystem.h"
#include "UIQuad.h"
#include "SceneSystem.h"
#include "Mesh.h"
#include "GraphicsDevice.h"
#include <vector>

GameObject* UI::CreateUI(GameObject* owner, const std::wstring& texturePath, const Vector3& position, const Vector3& scale,
	const std::string& vsPath, const std::string& psPath)
{
	// Scene の CreateGameObject を呼び出す
	GameObject* uiObject = owner->scenePtr()->CreateGameObject();

	// MeshRenderer を追加
	MeshRenderer* renderer = uiObject->AddComponent<MeshRenderer>();

	// メッシュ設定（RenderingSystem の CreateUIQuad を利用）
	// CreateUIQuad() の shared_ptr をキャッシュして寿命を保証する
	static std::vector<std::shared_ptr<Mesh>> s_uiMeshCache;
	auto meshPtr = owner->rendering().CreateUIQuad();
	s_uiMeshCache.push_back(meshPtr);
	renderer->mesh = s_uiMeshCache.back().get();

	renderer->renderQueue = RenderQueue::UI;

	// マテリアル設定（既存シェーダ名を使う）
	renderer->material.vsPath = vsPath;
	renderer->material.psPath = psPath;
	renderer->material.texturePath = texturePath;

	// 画面サイズとスクリーンスペースフラグをセット（UiVS に UseScreenSpace を実装している前提）
	GraphicsDevice& device = owner->rendering().GetGraphicsDevice();
	renderer->material.SetFloat("ScreenWidth", static_cast<float>(device.GetBackBufferWidth()));
	renderer->material.SetFloat("ScreenHeight", static_cast<float>(device.GetBackBufferHeight()));
	renderer->material.SetFloat("UseScreenSpace", 1.0f);

	// Transform 設定（位置/スケールはピクセル空間を想定）
	uiObject->transform().position() = position;
	uiObject->transform().scale() = scale;

	return uiObject;
}