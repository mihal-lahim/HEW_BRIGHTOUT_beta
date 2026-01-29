
#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H


class PhysicsSystem;
class Scene;
class ComponentSystem;
class RenderingSystem;

// ゲーム全体のコンテキスト情報を保持する構造体
struct GameContext
{
	Scene* currentScene;
	ComponentSystem* componentSystem;
	PhysicsSystem* physicsSystem;
	RenderingSystem* renderingSystem;
};


#endif
