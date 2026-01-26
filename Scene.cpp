#include "Scene.h"
#include "Time.h"

Scene::Scene()
{
	m_ObjectManager.Initialize();
	m_PhysicsSystem.Initialize();
	m_RenderSystem.Initialize();
}

Scene::~Scene()
{
	m_ObjectManager.Finalize();
	m_PhysicsSystem.Finalize();
	m_RenderSystem.Finalize();
}

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
