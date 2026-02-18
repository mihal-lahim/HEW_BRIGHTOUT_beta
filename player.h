/////////////////////////////
// player.h[プレイヤークラス]
//Author : hiroshi kasiwagi
//Date :2025/11/27
////////////////////////////////
#ifndef PLAYER_H
#define PLAYER_H

#include "ScriptComponent.h"
#include "GameObject.h"
#include "InputHandler.h"
#include "PlayerState.h"
#include "GamePad.h"
#include "TPSCamera.h"
#include "Health.h"
#include "PlayerMovement.h"
#include "PlayerMorphsystem.h"
#include "PlayerCommand.h"
#include "PlayerCommandSet.h"
#include "Renderer.h"
#include <vector>


#include "PlayerState_Electric.h"
#include "PlayerState_Human_Idle.h"
#include "PlayerState_Human_Walk.h"
#include "PlayerState_Human_MidAir.h"


class PlayerStateMachine;
class InputSystem;
class PlayerMovement;
class PlayerMorphSystem;
class TPSCamera;
class MeshRenderer;

// プレイヤーステート格納構造体
struct PlayerStates
{
	inline static PlayerState_Human_Idle HumanIdle{};
	inline static PlayerState_Human_Walk HumanWalk{};
	inline static PlayerState_Human_MidAir HumanMidAir{};
	inline static PlayerState_Electric Electric{};
};



class Player : public ScriptComponent
{
public:
	// 状態管理
	PlayerStateMachine* stateMachine = nullptr;

	// 入力システム
	InputHandler* inputHandler = nullptr;

	// 移動コンポーネント
	PlayerMovement* movement = nullptr;

	// 変身システム
	PlayerMorphSystem* morphSystem = nullptr;

	// カメラ
	TPSCamera* camera = nullptr;

	// 体力コンポーネント
	Health* health = nullptr;

	// 剛体
	PhysicsBody* physicsBody = nullptr;

	// モデルオブジェクト
	GameObject* modelObject = nullptr;

	// 歩行アニメーション用モデル
	std::vector<GameObject*> walkModelObjects{};
	// 歩行アニメーションの更新間隔
	float walkAnimationInterval = 1.0f;
	// 歩行アニメーションタイマー
	float walkAnimationTimer = 0.0f;
	// 歩行アニメーションインデックス
	size_t walkAnimationIndex = 0;


	void Start() override;
	void Update() override;
	void ResetWalkAnimation();
	void AdvanceWalkAnimation(float deltaTime);
};


#endif