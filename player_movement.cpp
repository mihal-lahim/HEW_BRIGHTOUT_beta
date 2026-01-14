#include "player_movement.h"
#include "player.h"
#include "PowerLine.h"
#include <algorithm>
#include "Camera.h"

using namespace DirectX;


XMVECTOR PlayerMovement::SetInputDir(float inputX, float inputZ)
{
	// 入力方向ベクトルを作成
	XMFLOAT3 inputVec = { inputX, 0.0f, inputZ };

	// 入力方向をカメラの向きに合わせて変換
	XMVECTOR convertedVec = ConvertToWorldFromInput(inputVec, m_Camera);

	// 最後の入力方向を保存
	m_Ctx.LastInputDir = convertedVec;

	return convertedVec;
}

void PlayerMovement::Start()
{
	// PoleManager取得
	m_PoleManager = GetOwner()->GetOwner()->GetGameObject<PoleManager>();
}

void PlayerMovement::Walk(float inputX, float inputZ)
{
	// 入力方向ベクトルを作成
	XMVECTOR vec = SetInputDir(inputX, inputZ);

	// 移動方向がゼロベクトルでなければ移動ベクトルを更新
	if (XMVectorGetX(vec) != 0.0f || XMVectorGetZ(vec) != 0.0f)
	{
		// Y成分を0にする
		vec = XMVectorSetY(vec, 0.0f);
		vec = XMVectorScale(XMVector3Normalize(vec), m_Ctx.WalkSpeed);

		// 現在の移動方向と新しい移動ベクトルを加算して設定
		SetMoveVec(vec + GetMoveVecV());
	}
}

void PlayerMovement::AirMove(float inputX, float inputZ)
{
	// 入力方向ベクトルを作成
	XMVECTOR vec = SetInputDir(inputX, inputZ);

	// 移動方向がゼロベクトルでなければ移動ベクトルを更新
	if (XMVectorGetX(vec) != 0.0f || XMVectorGetZ(vec) != 0.0f)
	{
		// Y成分を0にする
		vec = XMVectorSetY(vec, 0.0f);
		vec = XMVectorScale(XMVector3Normalize(vec), m_Ctx.WalkSpeed);

		// 現在の移動方向と新しい移動ベクトルを加算して設定
		SetMoveVec(vec + GetMoveVecV());
	}
}

void PlayerMovement::Jump(float inputX, float inputZ, float force)
{
	// 入力方向ベクトルを作成
	XMVECTOR inputDir = SetInputDir(inputX, inputZ);

	// 速度ベクトルのXZ成分
	XMVECTOR xzVel = XMVectorSetY(GetVelocityVecV(), 0.0f);

	// 速度ベクトルと入力した方向の内積を計算
	float dot = XMVectorGetX(XMVector3Dot(inputDir, XMVector3Normalize(xzVel)));

	// 内積に基づいてスケーリングファクターを計算
	float factor = m_Ctx.AirMinSpeedFactor + (1.0f - m_Ctx.AirMinSpeedFactor) * ((dot + 1.0f) * 0.5f);

	// XZ成分の速度ベクトルをスケーリング
	xzVel = XMVectorScale(xzVel, factor);

	// ジャンプベクトルを計算
	XMVECTOR jumpVel = XMVectorSetY(XMVectorZero(), XMVectorGetY(GetVelocityVecV()) + force);

	// 速度ベクトルを更新
	SetVelocityVec(XMVectorAdd(xzVel, jumpVel));
}

void PlayerMovement::GroundJump(float inputX, float inputZ)
{
	// ジャンプ処理を呼び出し
	Jump(inputX, inputZ, m_Ctx.JumpForce);
}

void PlayerMovement::ElectricJump(float inputX, float inputZ)
{
	// ジャンプ処理を呼び出し
	Jump(inputX, inputZ, m_Ctx.ElectricJumpForce);
}

void PlayerMovement::ApplyGravity(double elapsedTime)
{
	// 速度ベクトルを取得
	XMVECTOR vec = GetVelocityVecV();

	// Y成分に重力加速度を設定して加算
	XMVECTOR gravity = XMVectorZero();
	vec = XMVectorAdd(vec, XMVectorSetY(gravity, m_Ctx.Gravity * static_cast<float>(elapsedTime)));

	// 移動ベクトルを更新
	SetVelocityVec(vec);
}

void PlayerMovement::SnapToPowerLine(PowerLineID lineID)
{
	// 電線を取得
	PowerLine* powerLine = m_PoleManager->GetPowerLine(lineID);

	// 電線ベクトルを取得
	XMVECTOR lineVec = powerLine->GetLineVector();

	// 現在の速度ベクトルを取得
	XMVECTOR vec = GetVelocityVecV();

	// 速度ベクトルがゼロベクトルの場合
	if (XMVectorGetX(XMVectorEqual(vec, XMVectorZero())))
	{
		// 移動ベクトルを使用
		vec = GetMoveVecV();

		// 移動ベクトルもゼロベクトルの場合
		if (XMVectorGetX(XMVectorEqual(vec, XMVectorZero())))
		{
			// カメラ方向を設定
			vec = ConvertToWorldFromInput({ 0.0f, 0.0f, 1.0f }, m_Camera);
		}
	}


	// 速度ベクトルと電線ベクトルの内積を計算
	float dot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(lineVec), XMVector3Normalize(vec)));

	// 内積の符号に基づいて開始電柱と目的地電柱を設定
	m_Ctx.StartPole = dot >= 0.0f ? powerLine->GetPoles().first : powerLine->GetPoles().second;
	m_Ctx.DestPole = dot >= 0.0f ? powerLine->GetPoles().second : powerLine->GetPoles().first;

	// 電線IDを設定
	m_Ctx.LineID = lineID;


	// 電線ベクトルの半分の長さを取得
	XMVECTOR halfVec = XMVectorScale(lineVec, 0.5f);

	// 電線の中央位置を取得
	XMVECTOR lineMidPos = XMLoadFloat3(&powerLine->Transform.Position);

	// 開始位置を設定（内積の符号に基づく）
	XMVECTOR startPos = dot >= 0.0f ? XMVectorSubtract(lineMidPos, halfVec) : XMVectorAdd(lineMidPos, halfVec);

	// プレイヤーの現在位置を取得
	XMVECTOR playerPos = XMLoadFloat3(&GetOwner()->Transform.Position);

	// 開始位置からプレイヤー位置へのベクトルを取得
	XMVECTOR toPlayerVec = XMVectorSubtract(playerPos, startPos);
	
	// 電線ベクトルの長さを取得
	float lineLength = powerLine->GetLength();

	// 電線上の位置パラメータtを設定（0.0f ~ 1.0fの範囲にクランプ）
	m_Ctx.t = std::clamp(XMVectorGetX(XMVector3Dot(toPlayerVec, lineVec)) / lineLength, 0.0f, 1.0f);



	// 速度ベクトルの長さを取得
	float velocityLength = XMVectorGetX(XMVector3Length(vec));

	// 電線上速度を設定
	m_Ctx.LineMoveSpeed = velocityLength >= m_Ctx.LineMoveSpeedMin ? velocityLength : m_Ctx.LineMoveSpeedMin;
}

void PlayerMovement::Turn(float inputX, float inputZ)
{
	// 現在の電線を取得
	PowerLine* powerLine = m_PoleManager->GetPowerLine(m_Ctx.LineID);

	// 接続されている電柱IDペアを取得
	std::pair<PoleID, PoleID> poles = powerLine->GetPoles();

	// 電線ベクトルを取得
	XMVECTOR lineVec = m_Ctx.StartPole == poles.first ? powerLine->GetLineVector() : XMVectorNegate(powerLine->GetLineVector());

	// 入力方向ベクトルを作成
	XMVECTOR vec = SetInputDir(inputX, inputZ);

	// 電線ベクトルと入力方向ベクトルの内積を計算
	float dot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(lineVec), XMVector3Normalize(vec)));

	// 内積が負の場合、方向を反転
	if (dot < 0.0f)
	{
		// 目的地と開始地点を入れ替え
		std::swap(m_Ctx.StartPole, m_Ctx.DestPole);

		// パラメータtを反転
		m_Ctx.t = 1.0f - m_Ctx.t;
	}
}

void PlayerMovement::Move()
{
	// 次の電柱に到達したか判定
	if (m_Ctx.t >= 1.0f)
	{
		// 到達したら目的地の電柱を開始電柱に設定
		m_Ctx.StartPole = m_Ctx.DestPole;
		// 次の目的地の電柱を取得
		m_Ctx.DestPole = m_PoleManager->GetDirectionalPole(m_Ctx.StartPole, m_Ctx.LastInputDir);
		// tをリセット
		m_Ctx.t = 0.0f;
		// 新しい電線IDを取得
		m_Ctx.LineID = m_PoleManager->GetPowerLineID(m_Ctx.StartPole, m_Ctx.DestPole);
	}

	// tを進める
	float lineLength = m_PoleManager->GetPowerLineLength(m_Ctx.LineID);
	m_Ctx.t += (m_Ctx.LineMoveSpeed / lineLength);

	// 電線上の位置を取得してプレイヤーの位置を更新
	XMFLOAT3 newPos = m_PoleManager->GetPositionOnPowerLine(m_Ctx.StartPole, m_Ctx.DestPole, m_Ctx.t);
	SetMoveVec(newPos);
}

void PlayerMovement::Eject(float inputX, float inputZ)
{
	// 電線上からの射出処理
	ElectricJump(inputX, inputZ);
}


XMVECTOR ConvertToWorldFromInput(const XMFLOAT3 inputDir, const Camera* camera)
{
	// 入力方向のベクトル
	XMVECTOR inputVec = XMLoadFloat3(&inputDir);

	// カメラの回転クォータニオンを取得
	XMVECTOR rot = XMLoadFloat4(&camera->GetOwner()->Transform.Rotation.Quat);

	// 入力方向ベクトルを回転させてワールド座標系に変換
	return XMVector3Rotate(inputVec, rot);
}
