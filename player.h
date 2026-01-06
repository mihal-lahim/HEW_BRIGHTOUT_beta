/////////////////////////////
// player.h[プレイヤークラス]
//Author : hiroshi kasiwagi
//Date :2025/11/27
////////////////////////////////
#ifndef PLAYER_H
#define PLAYER_H

#include <DirectXMath.h>
#include "GameObject.h"
#include "input_system.h"
#include "tps_camera.h"
#include "health.h"
#include "model.h"
#include "player_movement.h"
#include "player_state.h"
#include "PoleManager.h"
#include "player_morphsystem.h"
#include "player_command.h"
#include "player_states_instances.h"



class Player : public GameObject
{
private:

    // 生存フラグ
    bool m_IsAlive = true;

    // モデル
    MODEL* m_HumanModel = nullptr;
    //電気状態用モデル
    MODEL* m_ElectricModel = nullptr;

    // InputSystem
	InputSystem* m_InputSystem = nullptr;

    // カメラ
    TPSCamera* m_Camera = nullptr;

    // 体力
    Health* m_Health = nullptr;

    // 移動
	PlayerMovement* m_Movement = nullptr;

	// ポールマネージャー
	PoleManager* m_PoleManager = nullptr;

	// 変身システム
	PlayerMorphSystem* m_MorphSystem = nullptr;

	// 状態管理
	PlayerStateMachine* m_StateMachine = nullptr;

    // 地面に接しているかどうか
	bool m_IsGrounded = false;

public:
    Player(InputSystem* inputSystem, PoleManager* poleManager);
	~Player() = default;

    // 毎フレーム更新
    void Update(double elapsedSec) override;

	// 描画処理
	void Draw() const override;

    // Cameraの設定・取得
	void SetTPSCamera(TPSCamera* camera) { m_Camera = camera; }
	TPSCamera* const GetTPSCamera() const { return m_Camera; }


    // InputSystemの設定・取得
    void SetInputSystem(InputSystem* inputSystem) { m_InputSystem = inputSystem; }
    const InputSystem* const GetInputSystem() const { return m_InputSystem; }

	// モデル設定・取得
	void SetHumanModel(MODEL* humanModel) { m_HumanModel = humanModel; }
	MODEL* const GetHumanModel() const { return m_HumanModel; }
    void SetElectricModel(MODEL* electricModel) { m_ElectricModel = electricModel; }
	MODEL* const GetElectricModel() const { return m_ElectricModel; }

    
	// Healthコンポーネントの設定・取得
	void SetHealth(Health* health) { m_Health = health; }
	Health* const GetHealth() const { return m_Health; }

	// Movementコンポーネントの取得・設定
	void SetMovement(PlayerMovement* movement) { m_Movement = movement; }
	PlayerMovement* const GetMovement() const { return m_Movement; }

	// PoleManagerの取得・設定
	void SetPoleManager(PoleManager* poleManager) { m_PoleManager = poleManager; }
	const PoleManager* const GetPoleManager() const { return m_PoleManager; }

	// MorphSystemの取得・設定
	void SetMorphSystem(PlayerMorphSystem* morphSystem) { m_MorphSystem = morphSystem; }
	PlayerMorphSystem* const GetMorphSystem() const { return m_MorphSystem; }

	// 状態管理コンポーネントの取得・設定
	void SetStateMachine(PlayerStateMachine* stateMachine) { m_StateMachine = stateMachine; }
	const PlayerStateMachine* GetStateMachine() const { return m_StateMachine; }
    void ChangeState(PlayerState* newState) { m_StateMachine->ChangeState(*this, newState); }

	// 地面接地状態の設定・取得
	void SetGrounded(bool isGrounded) { m_IsGrounded = isGrounded; }
	bool IsGrounded() const { return m_IsGrounded; }
};


#endif