#ifndef PLAYER_MOVEMENT_H
#define PLAYER_MOVEMENT_H

#include <DirectXMath.h>
#include "Movement.h"
#include "PoleManager.h"
#include "Ray.h"


class Player;
class Camera;

// プレイヤー移動設定値構造体
struct PlayerMoveCtx
{
	// 歩行速度
	float WalkSpeed = 5.0f;
	// 空中移動速度
	float AirMoveSpeed = 3.0f;
	// 空中最小速度の倍率
	float AirMinSpeedFactor = 0.2f;

	// ジャンプ力
	float JumpForce = 20.0f;
	// 電気ジャンプ力
	float ElectricJumpForce = 10.0f;

	// 電線上移動速度
	float LineMoveSpeed = 20.0f;
	// 電線上移動速度の最小値
	float LineMoveSpeedMin = 1.0f;

	// レイキャストオフセット距離
	float RayCastOffset = 0.0f;
	// 地面検出オフセット距離
	float GroundDetectOffset = 0.5f;
	// レイの長さ
	float RayLength = 10.0f;

	// 重力加速度
	float Gravity = -30.0f;

};


// プレイヤー移動クラス
class PlayerMovement : public Movement
{
public:
	// 地面判定処理
	bool IsOnGround() const;

	// レイキャスト更新処理
	void UpdateRayCast();

	// 重力適用処理
	void ApplyGravity();

	// 地上移動処理
	void GroundMove(float inputX, float inputZ, float velocity);
	// 歩行処理
	void Walk(float inputX, float inputZ);
	// 走行処理
	void Run(float inputX, float inputZ);
	// 空中移動処理
	void AirMove(float inputX, float inputZ);
	// ジャンプ呼び出し処理
	void Jump(float inputX, float inputZ, float force);
	// 地上ジャンプ処理
	void GroundJump(float inputX, float inputZ);
	// 電気ジャンプ処理
	void ElectricJump(float inputX, float inputZ);


	// 電線上に移動する処理
	void SnapToPowerLine(PowerLineID lineID);
	// 電線上方向指定処理
	void Turn(float inputX, float inputZ);
	// 電線上移動処理
	void LineMove();
	// 電線射出処理
	void Eject(float inputX, float inputZ);

	// PoleManager
	void SetPoleManager(PoleManager* poleManager) { m_PoleManager = poleManager; }

	// 移動設定値取得・設定
	void SetMoveCtx(const PlayerMoveCtx& moveCtx) { m_Ctx = moveCtx; }
	const PlayerMoveCtx& GetMoveCtx() const { return m_Ctx; }

	// コンストラクタ
	PlayerMovement(Camera* camera, PlayerMoveCtx moveCtx = {})
		: m_Camera(camera), m_Ctx(moveCtx), m_LineMoveSpeed(moveCtx.LineMoveSpeed)
	{
	}

	// 更新処理
	void PostUpdate() override;
private:
	// 移動設定値
	PlayerMoveCtx m_Ctx;

	// カメラ
	Camera* m_Camera = nullptr;

	// PoleManager
	PoleManager* m_PoleManager = nullptr;

	// 地面判定用レイキャスト
	Ray m_GroundRay{ {},{} };


	// 電柱
	PoleID m_StartPole = -1;
	PoleID m_DestPole = -1;

	// 電線ID
	PowerLineID m_LineID = -1;

	// 電線上の位置パラメータ
	float m_LineParam = 0.0f;

	// 電線上移動速度
	float m_LineMoveSpeed = 0.0f;

	// 最後の入力方向
	Vector3 m_LastInputDir{};

	// 入力方向を保存
	Vector3 SetInputDir(float inputX, float inputZ);

	//歩いているかどうか
	bool m_IsMoving = false;
};

// 入力方向をワールド座標系に変換する関数
Vector3 ConvertToWorldFromInput(const Vector3& inputDir, const Camera* camera);

// ワールド座標系の方向ベクトルをXZ平面上のベクトルに変換する関数
Vector3 ConvertToXZPlane(const Vector3& worldDir);

#endif
