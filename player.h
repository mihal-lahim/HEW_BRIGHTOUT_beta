/////////////////////////////
// player.h[プレイヤークラス]
//Author : hiroshi kasiwagi
//Date :2025/11/27
////////////////////////////////
#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "InputSystem.h"
#include "PlayerState.h"
#include "ObjectManager.h"
#include "XBOXController.h"
#include "TPSCamera.h"
#include "Health.h"
#include "PlayerMovement.h"
#include "PlayerMorphsystem.h"
#include "PlayerCommand.h"
#include "PlayerCommandSet.h"
#include "MeshRenderer.h"


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
struct MODEL;

// プレイヤーステート格納構造体
struct PlayerStates
{
	inline static PlayerState_Human_Idle HumanIdle{};
	inline static PlayerState_Human_Walk HumanWalk{};
	inline static PlayerState_Human_MidAir HumanMidAir{};
	inline static PlayerState_Electric Electric{};
};



class Player : public GameObject
{
public:
	// 状態管理
	PlayerStateMachine* m_StateMachine = nullptr;

	// 入力システム
	InputSystem* m_InputSystem = nullptr;

	// 移動コンポーネント
	PlayerMovement* m_Movement = nullptr;

	// 変身システム
	PlayerMorphSystem* m_MorphSystem = nullptr;

	// カメラ
	TPSCamera* m_Camera = nullptr;

	// 体力コンポーネント
	Health* m_Health = nullptr;

	// 剛体
	RigidBody* m_RigidBody = nullptr;

	// メッシュレンダラー
	MeshRenderer* m_MeshRenderer = nullptr;



	// 人間モデル
	MODEL* m_HumanModel = nullptr;

	// 電気モデル
	MODEL* m_ElectricModel = nullptr;



	// コンストラクタ
	Player();
	~Player();

	void Awake() override;
	void Start() override;
	void Update() override;
};


#endif