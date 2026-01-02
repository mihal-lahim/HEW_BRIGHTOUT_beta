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
#include "camera.h"
#include "health.h"
#include "model.h"
#include "player_movement.h"
#include "player_state.h"
#include "PoleManager.h"
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
	InputSystem m_InputSystem;

    // カメラ
    Camera* m_Camera;

    // 体力
    Health m_Health;

    // 移動
	PlayerMovement* m_Movement;

	// ポールマネージャー
	PoleManager* m_PoleManager;

	// 状態管理
	PlayerStateMachine m_StateMachine;

    // 地面に接しているかどうか
	bool m_IsGrounded = false;

public:
    Player();
    ~Player();

    Player(MODEL* model, MODEL* electricModel, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir);

    // 毎フレーム更新（経過時間は秒）
    void Update(double elapsedSec) override;


    // カメラを生成（target の y 成分を高さと解釈）
    void CreateCamera(const DirectX::XMFLOAT3& target);
	const Camera* GetCamera() const { return m_Camera; }


    // InputSystemの設定・取得
    void SetInputSystem(InputSystem inputSystem) { m_InputSystem = inputSystem; }
    const InputSystem& GetInputSystem() const { return m_InputSystem; }

	// モデル設定
	void SetHumanModel(MODEL* humanModel) { m_HumanModel = humanModel; }
    void SetElectricModel(MODEL* electricModel) { m_ElectricModel = electricModel; }

    
	// Healthコンポーネントの設定・取得
	void SetHealth(Health health) { m_Health = health; }
	const Health& GetHealth() const { return m_Health; }

	// Movementコンポーネントの取得・設定
	void SetMovement(PlayerMovement* movement) { m_Movement = movement; }
	PlayerMovement* const GetMovement() const { return m_Movement; }

	// PoleManagerの取得・設定
	void SetPoleManager(PoleManager* poleManager) { m_PoleManager = poleManager; }
	PoleManager* const GetPoleManager() const { return m_PoleManager; }

	// 状態管理コンポーネントの取得・設定
	void SetStateMachine(PlayerStateMachine stateMachine) { m_StateMachine = stateMachine; }
	const PlayerStateMachine& GetStateMachine() const { return m_StateMachine; }
    void ChangeState(PlayerState* newState) { m_StateMachine.ChangeState(*this, newState); }

	// 地面接地状態の設定・取得
	void SetGrounded(bool isGrounded) { m_IsGrounded = isGrounded; }
	bool IsGrounded() const { return m_IsGrounded; }

    // 描画処理
    void Draw() const;
};

#endif