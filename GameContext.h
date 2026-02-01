
#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

class WindowSystem;
class SceneSystem;
class PhysicsSystem;

class InputSystem;
class RenderingSystem;

// ゲーム全体のコンテキスト情報を保持する構造体
struct GameContext
{
	WindowSystem* windowSystem = nullptr;
	SceneSystem* sceneSystem = nullptr;
	PhysicsSystem* physicsSystem = nullptr;

	InputSystem* inputSystem = nullptr;
	RenderingSystem* renderingSystem = nullptr;
};


#endif
