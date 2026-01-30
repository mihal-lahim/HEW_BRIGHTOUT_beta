////////////////////////////////////
//シーン管理マネージャー
//Author :hiroshi kasiwagi
//Date   :2025/12/20
////////////////////////////////////
#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "SceneSystem.h"
#include "PhysicsSystem.h"
#include "RenderingSystem.h"
#include "InputSystem.h"
#include <memory>

struct GameContext;

class EngineCore
{
private:
	// シーンシステムポインタ
	std::unique_ptr<SceneSystem> m_sceneSystem = nullptr;

	// 物理システムポインタ
	std::unique_ptr<PhysicsSystem> m_physicsSystem = nullptr;

	// 入力システムポインタ
	std::unique_ptr<InputSystem> m_inputSystem = nullptr;

	// レンダリングシステムポインタ
	std::unique_ptr<RenderingSystem> m_renderingSystem = nullptr;

public:
	
	// 現在のゲームコンテキスト取得メソッド
	GameContext& GetGameContext();

	// エンジンコアの初期化
	void Initialize();

	// エンジンコアの更新
	void Update();

	// エンジンコアの終了処理
	void Finalize();
};

#endif
