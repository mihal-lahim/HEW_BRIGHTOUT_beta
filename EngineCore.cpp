#include "EngineCore.h"
#include "GameContext.h"


GameContext& EngineCore::GetGameContext()
{
	static GameContext gameContext{};

	gameContext.windowSystem = m_windowSystem.get();
	gameContext.sceneSystem = m_sceneSystem.get();
	gameContext.physicsSystem = m_physicsSystem.get();

	gameContext.inputSystem = m_inputSystem.get();
	gameContext.renderingSystem = m_renderingSystem.get();

	return gameContext;
}

void EngineCore::Initialize()
{
	m_windowSystem = std::make_unique<WindowSystem>(this);
	m_sceneSystem = std::make_unique<SceneSystem>(this);
	m_physicsSystem = std::make_unique<PhysicsSystem>(this);

	m_inputSystem = std::make_unique<InputSystem>(this);
	m_renderingSystem = std::make_unique<RenderingSystem>(this);
}

void EngineCore::Update()
{
	m_inputSystem->Update();

	m_sceneSystem->Update();

	m_renderingSystem->Render(m_sceneSystem->currentScene());
}

void EngineCore::Finalize()
{
	m_windowSystem.reset();
	m_sceneSystem.reset();
	m_physicsSystem.reset();

	m_inputSystem.reset();
	m_renderingSystem.reset();
}
