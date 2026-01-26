#include "Scene.h"
#include "Time.h"

void Scene::Update()
{
	Time::BeginFrame();

	// 物理演算の固定更新を処理
	while (Time::HasFixedStep())
	{
		Time::ConsumeFixedStep();
		m_PhysicsSystem.PhysicsUpdate((float)Time::FixedDeltaTime());
	}

	// オブジェクト管理システムの更新
	m_ObjectManager.Cycle();

	// レンダリングシステムの更新
	m_RenderSystem.Render();
}
