/////////////////////////////
// player.h[プレイヤークラス]
//Author : hiroshi kasiwagi
//Date :2025/11/27
////////////////////////////////
#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "input_system.h"
#include "PlayerSystem.h"
#include "player_state.h"
#include "ObjectManager.h"
#include "controller.h"
#include "input_system.h"
#include "tps_camera.h"
#include "health.h"
#include "player_movement.h"
#include "player_morphsystem.h"
#include "player_command.h"
#include "PlayerSystem.h"
#include "MeshRenderer.h"


class Player : public GameObject
{
public:
<<<<<<< HEAD
    Player();
	~Player() = default;
=======
 Player();
 ~Player();
 explicit Player(const DirectX::XMFLOAT3& pos);

 Player(MODEL* model, MODEL* electricModel, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir);

 // 毎フレーム更新（経過時間は秒）
 void Update(double elapsedSec) override;

 // 移動：方向は正規化されていることを想定
 void Move(const DirectX::XMFLOAT3& dir, double elapsedSec);

 // ダッシュ開始/終了
 void StartDash();
 void StopDash();
 
 // ダメージ/回復
 void TakeDamage(float amount);
 void Heal(float amount);

 // コントローラ/カメラ管理
 void SetController(Controller* controller);
 Controller* GetController() const;

 // カメラを生成（target の y 成分を高さと解釈）
 void CreateCamera(const DirectX::XMFLOAT3& target);
 Camera* GetCamera() const; // return base Camera pointer for external use

 //位置取得（カメラ等が参照するためのアクセサ）
 const DirectX::XMFLOAT3& GetPosition() const { return m_Position; }

 //体力取得
 float GetHealth() const { return health_; }
 float GetMaxHealth() const { return maxHealth_; }

 //方向/速度設定
 void SetDirection(const DirectX::XMFLOAT3& dir) { direction_ = dir; }
 DirectX::XMFLOAT3 GetDirection() const { return direction_; }
 void SetBaseSpeed(float speed) { baseSpeed_ = speed; if (!isDashing_) currentSpeed_ = speed; }

 // AABB チェック用取得
 AABB GetAABB() const;

 //死亡判定
 bool IsAlive() const { return health_ > 0 && usePlayer; }

 // プレイヤーステート変更
 void ChangeState(Player::State newState);

 Player::State GetState() const { return state; }

 void SetElectricModel(MODEL* electricModel) { electricModel_ = electricModel; }

 // 衝突判定
 bool IsCollidingWith(const AABB& other) const { return GetAABB().IsOverlap(other); }

 // ジャンプ処理
 void Jump(float jumpForce = 5.0f);
 bool IsGrounded() const { return isGrounded_; }

 // 電柱周辺検出（電気状態への変身可能判定）
 bool IsNearPole() const;
 
 // 電線への自動スナップ（電気状態で移動）
 void SnapToNearestPowerLine();
 
 // 電気状態から人間に変化する際、電柱から跳ね返す
 void KnockbackFromPole();

 // ハウスへの電気供給
 void TransferElectricityToHouse(class House* house, double elapsedSec);

 // 最も近いハウスを取得
 class House* GetNearestHouse() const;

 // 供給中かどうかを判定
 bool IsSupplyingElectricity() const { return m_isSupplying; }
 
 // 供給中のハウスを取得
 class House* GetSupplyingHouse() const { return m_supplyingHouse; }

 // 供給開始/停止
 void StartSupplyingElectricity(class House* house);
 void StopSupplyingElectricity();

 // 描画処理
 void Draw() const;

private:
    // 衝突解決ヘルパー
    void ResolveCollisions(DirectX::XMFLOAT3& newPos/*, double elapsedSec*/);
    //AABB取得ヘルパー
    AABB GetAABBAt(const DirectX::XMFLOAT3& pos) const;

    // 電気状態変化時のリセット処理
    void ResetToElectricityState();

    // 電柱检测用の定数
    static constexpr float POLE_DETECTION_RADIUS = 3.0f;  
    static constexpr float POWER_LINE_SNAP_DISTANCE = 2.0f;
    
    // 電気状態から変化時の跳ね返り定数
    static constexpr float KNOCKBACK_DISTANCE = 3.0f;      // 電柱から跳ね返す距離（メートル）
    static constexpr float KNOCKBACK_JUMP_FORCE = 8.0f;    // 跳ね返し時の上昇初速度
    static constexpr float KNOCKBACK_HORIZONTAL_SPEED = 5.0f; // 水平方向の跳ね返し速度

>>>>>>> parent of 4f2a780 (inertiaと射出アクションの追加(完了))
};


#endif