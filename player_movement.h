#ifndef PLAYER_MOVEMENT_H
#define PLAYER_MOVEMENT_H

#include <DirectXMath.h>
#include "movement.h"
#include "PoleManager.h"

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
	float JumpForce = 10.0f;
	// 電気ジャンプ力
	float ElectricJumpForce = 20.0f;

	// 重力加速度
	float Gravity = -9.8f;

	// 電柱
	PoleID StartPole = -1;
	PoleID DestPole = -1;
	// 電線ID
	PowerLineID LineID = -1;
	// 電線上の位置パラメータ
	float t = 0.0f;
	// 電線上移動速度
	float LineMoveSpeed = 0.0f;
	// 電線上移動速度の最小値
	float LineMoveSpeedMin = 10.0f;

	// 最後の入力方向
	DirectX::XMVECTOR LastInputDir{};
};


// プレイヤー移動クラス
class PlayerMovement : public Movement
{
private:
	// 移動設定値
	PlayerMoveCtx m_Ctx;

	// カメラ
	Camera* m_Camera = nullptr;

	// PoleManager
	PoleManager* m_PoleManager = nullptr;

	// 入力方向を保存
	DirectX::XMVECTOR SetInputDir(float inputX, float inputZ);
public:

	void Start() override;

	// 歩行処理
	void Walk(float inputX, float inputZ);
	// 空中移動処理
	void AirMove(float inputX, float inputZ);
	// ジャンプ呼び出し処理
	void Jump(float inputX, float inputZ, float force);
	// 地上ジャンプ処理
	void GroundJump(float inputX, float inputZ);
	// 電気ジャンプ処理
	void ElectricJump(float inputX, float inputZ);
	// 重力適用
	void ApplyGravity(double elapsedTime);


	// 電線上に移動する処理
	void SnapToPowerLine(PowerLineID lineID);
	// 電線上方向指定処理
	void Turn(float inputX, float inputZ);
	// 電線上移動処理
	void Move();
	// 電線射出処理
	void Eject(float inputX, float inputZ);

	// 移動設定値取得・設定
	void SetMoveCtx(const PlayerMoveCtx& moveCtx) { m_Ctx = moveCtx; }
	const PlayerMoveCtx& GetMoveCtx() const { return m_Ctx; }

	// コンストラクタ
	PlayerMovement(Camera* camera, PlayerMoveCtx moveCtx = {})
		: m_Camera(camera), m_Ctx(moveCtx)
	{}
};

// 入力方向をワールド座標系に変換する関数
DirectX::XMVECTOR ConvertToWorldFromInput(const DirectX::XMFLOAT3 inputDir, const Camera* camera);


#endif
