#include "Title.h"
#include "GameObject.h"
#include "TitleController.h"
#include "RenderingSystem.h"

void Title::Initialize()
{
	// デフォルトの背景色に戻す
	rendering().GetGraphicsDevice().SetClearColor(0.5f, 0.5f, 0.5f);

	// タイトルシーンの制御用GameObjectを作成
	GameObject* controller = CreateGameObject();
	controller->AddComponent<TitleController>();
}
