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
	// 電線移動時の画像エフェクトオブジェクト
	GameObject* electricEffectObject = nullptr;
	// 移動速度バフ表示テキストのルート
	GameObject* moveSpeedTextRoot = nullptr;
	// 復旧速度バフ表示テキストのルート
	GameObject* repairSpeedTextRoot = nullptr;

	// 歩行アニメーション用モデル
	std::vector<GameObject*> walkModelObjects{};
	// 待機アニメーション用モデル
	std::vector<GameObject*> idleModelObjects{};
	// 射撃アニメーション用モデル
	std::vector<GameObject*> fireModelObjects{};
	// 歩行アニメーションの更新間隔
	float walkAnimationInterval = 1.0f;
	// 歩行アニメーションタイマー
	float walkAnimationTimer = 0.0f;
	// 歩行アニメーションインデックス
	size_t walkAnimationIndex = 0;
	// 待機アニメーションタイマー
	float idleAnimationTimer = 0.0f;
	// 待機アニメーションインデックス
	size_t idleAnimationIndex = 0;
	// 射撃アニメーションタイマー
	float fireAnimationTimer = 0.0f;
	// 射撃アニメーションインデックス
	size_t fireAnimationIndex = 0;

	// 電線移動画像アニメーションの更新間隔
	float electricAnimationInterval = 0.08f;
	// 電線移動画像アニメーションタイマー
	float electricAnimationTimer = 0.0f;
	// 電線移動画像アニメーションインデックス
	size_t electricAnimationIndex = 0;
	// 電線移動画像アニメーションの分割数
	int electricSheetColumns = 4;
	int electricSheetRows = 4;
	int electricSheetFrameCount = 16;


	float FireInterval = 0.3f;
	float BulletSpeed = 20.0f;
	float BulletLifeTime = 3.0f;
	float FallReturnY = -20.0f;
	float IdleAnimationInterval = 0.35f;
	float FireAnimationInterval = 0.06f;
	float FireAnimationDuration = 0.12f;


	void Start() override;
	void Update() override;
	void HandleFire();
	void FireBullet();
	void SetHumanVisualActive(bool active);
	void SetElectricEffectActive(bool active);
	void ResetWalkAnimation();
	void AdvanceWalkAnimation(float deltaTime);
	void UpdateHumanPseudoAnimation(float deltaTime);
	void SetFacingYaw(float yaw);
	void ApplyFacingYaw();
	void HideAllHumanPseudoAnimationModels();
	void ResetHumanPseudoAnimationGroup(std::vector<GameObject*>& models, size_t& index, float& timer);
	void AdvanceHumanPseudoAnimationGroup(std::vector<GameObject*>& models, size_t& index, float& timer, float interval, float deltaTime);
	void ResetElectricAnimation();
	void AdvanceElectricAnimation(float deltaTime);
	void ApplyElectricFrameUV(size_t frameIndex);
	void ApplyPermanentMoveSpeedBuff();
	void ApplyPermanentRestoreSpeedBuff();
	float GetRestoreSpeedMultiplier() const;
	void ShowMoveSpeedBuffText();
	void UpdateMoveSpeedBuffText(float deltaTime);
	void ShowRepairSpeedBuffText();
	void UpdateRepairSpeedBuffText(float deltaTime);

	int MoveSpeedBuffStack = 0;
	float MoveSpeedBuffPerRestore = 0.5f;
	int RestoreSpeedBuffStack = 0;
	float RestoreSpeedBuffPerRestore = 0.2f;
	float MoveSpeedBuffTextDuration = 1.2f;
	float RepairSpeedBuffTextDuration = 1.2f;

private:
	float m_FireTimer = 0.0f;
	float m_FireAnimationTimer = 0.0f;
	float m_LastFacingYaw = 0.0f;
	float m_MoveSpeedBuffTextTimer = 0.0f;
	float m_RepairSpeedBuffTextTimer = 0.0f;
	enum class HumanPseudoAnimationMode
	{
		Idle,
		Walk,
		Fire,
	};
	HumanPseudoAnimationMode m_HumanPseudoAnimationMode = HumanPseudoAnimationMode::Idle;
	bool m_IsReturningToTitle = false;
};


#endif