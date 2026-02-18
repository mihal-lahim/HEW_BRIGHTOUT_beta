#include "Result.h"
#include "GameObject.h"
#include "ResultController.h"
#include "RenderingSystem.h"

void Result::Initialize()
{
	// リザルトシーンの背景色を変更（青系の色で区別しやすくする）
	rendering().GetGraphicsDevice().SetClearColor(0.1f, 0.2f, 0.5f);

	// リザルトシーンの制御用GameObjectを作成
	GameObject* controller = CreateGameObject();
	controller->AddComponent<ResultController>();
}
