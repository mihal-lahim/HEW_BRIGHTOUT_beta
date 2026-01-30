
#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H


class SceneSystem;
class PhysicsSystem;

class InputSystem;
class RenderingSystem;

// ゲーム全体のコンテキスト情報を保持する構造体
struct GameContext
{
	SceneSystem* sceneSystem;
	PhysicsSystem* physicsSystem;

	InputSystem* inputSystem;
	RenderingSystem* renderingSystem;
};


#endif
