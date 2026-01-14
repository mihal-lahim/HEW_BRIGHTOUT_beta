
#ifndef PLAYER_SYSTEM_H
#define PLAYER_SYSTEM_H

#include "Component.h"
#include "player_state_electric.h"
#include "player_state_human_idle.h"
#include "player_state_human_walk.h"
#include "player_state_human_midair.h"


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
	inline static PlayerState_Human_Idle* HumanIdle{};
	inline static PlayerState_Human_Walk* HumanWalk{};
	inline static PlayerState_Human_MidAir* HumanMidAir{};
	inline static PlayerState_Electric* Electric{};

};

class PlayerSystem : public Component
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

	// メッシュレンダラー
	MeshRenderer* m_MeshRenderer = nullptr;

	// 人間モデル
	MODEL* m_HumanModel = nullptr;

	// 電気モデル
	MODEL* m_ElectricModel = nullptr;

	// コンストラクタ
	PlayerSystem(TPSCamera* camera)
		: m_Camera(camera)
	{}

	void Start() override;
	void Update(double elapsedTime) override;
};



#endif