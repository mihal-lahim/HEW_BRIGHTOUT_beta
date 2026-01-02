#include "player_movement.h"
#include "player.h"
#include "PowerLine.h"

using namespace DirectX;




XMVECTOR PlayerMovement::SetInputDir(float inputX, float inputZ, const Player* player)
{
	// 入力方向ベクトルを作成
	XMFLOAT3 inputVec = { inputX, 0.0f, inputZ };

	// 入力方向をカメラの向きに合わせて変換
	XMFLOAT3 convertedVec = ConvertToWorldFromInput(inputVec, player->GetCamera());

	// 最後の入力方向を保存
	m_MoveCtx.LastInputDir = XMLoadFloat3(&convertedVec);
}

void PlayerMovement::Walk(float inputX, float inputZ, const Player* player)
{
	// 入力方向ベクトルを作成
	XMVECTOR vec = SetInputDir(inputX, inputZ, player);

	// 移動方向がゼロベクトルでなければ移動ベクトルを更新
	if (XMVectorGetX(vec) != 0.0f || XMVectorGetZ(vec) != 0.0f)
	{
		// Y成分を0にする
		vec = XMVectorSetY(vec, 0.0f);
		vec = XMVectorScale(XMVector3Normalize(vec), m_MoveCtx.WalkSpeed);

		// 現在の移動方向と新しい移動ベクトルを加算して設定
		SetMoveVec(vec + GetMoveVecV());
	}
}

void PlayerMovement::AirMove(float inputX, float inputZ, const Player* player)
{
	// 入力方向ベクトルを作成
	XMVECTOR vec = SetInputDir(inputX, inputZ, player);

	// 移動方向がゼロベクトルでなければ移動ベクトルを更新
	if (XMVectorGetX(vec) != 0.0f || XMVectorGetZ(vec) != 0.0f)
	{
		// Y成分を0にする
		vec = XMVectorSetY(vec, 0.0f);
		vec = XMVectorScale(XMVector3Normalize(vec), m_MoveCtx.WalkSpeed);

		// 現在の移動方向と新しい移動ベクトルを加算して設定
		SetMoveVec(vec + GetMoveVecV());
	}
}

void PlayerMovement::Jump(float inputX, float inputZ, float force, const Player* player)
{
	// 入力方向ベクトルを作成
	XMVECTOR inputDir = SetInputDir(inputX, inputZ, player);

	// 速度ベクトルのXZ成分
	XMVECTOR xzVel = XMVectorSetY(GetVelocityVecV(), 0.0f);

	// 速度ベクトルと入力した方向の内積を計算
	float dot = XMVectorGetX(XMVector3Dot(inputDir, XMVector3Normalize(xzVel)));

	// 内積に基づいてスケーリングファクターを計算
	float factor = m_MoveCtx.AirMinSpeedFactor + (1.0f - m_MoveCtx.AirMinSpeedFactor) * ((dot + 1.0f) * 0.5f);

	// XZ成分の速度ベクトルをスケーリング
	xzVel = XMVectorScale(xzVel, factor);

	// ジャンプベクトルを計算
	XMVECTOR jumpVel = XMVectorSetY(XMVectorZero(), XMVectorGetY(GetVelocityVecV()) + force);

	// 速度ベクトルを更新
	SetVelocityVec(XMVectorAdd(xzVel, jumpVel));
}

void PlayerMovement::GroundJump(float inputX, float inputZ, const Player* player)
{
	// ジャンプ処理を呼び出し
	Jump(inputX, inputZ, m_MoveCtx.JumpForce, player);
}

void PlayerMovement::ElectricJump(float inputX, float inputZ, const Player* player)
{
	// ジャンプ処理を呼び出し
	Jump(inputX, inputZ, m_MoveCtx.ElectricJumpForce, player);
}

void PlayerMovement::ApplyGravity(double elapsedTime)
{
	// 速度ベクトルを取得
	XMVECTOR vec = GetVelocityVecV();

	// Y成分に重力加速度を設定して加算
	XMVECTOR gravity = XMVectorZero();
	vec = XMVectorAdd(vec, XMVectorSetY(gravity, m_MoveCtx.Gravity * static_cast<float>(elapsedTime)));

	// 移動ベクトルを更新
	SetVelocityVec(vec);
}

void PlayerMovement::Turn(float inputX, float inputZ, const Player* player)
{
	// PoleManagerを取得
	PoleManager* poleManager = player->GetPoleManager();

	// 現在の電線を取得
	PowerLine* powerLine = poleManager->GetPowerLine(m_MoveCtx.LineID);

	// 接続されている電柱IDペアを取得
	std::pair<PoleID, PoleID> poles = powerLine->GetPoles();

	// 電線ベクトルを取得
	XMVECTOR lineVec = m_MoveCtx.StartPole == poles.first ? powerLine->GetLineVector() : XMVectorNegate(powerLine->GetLineVector());

	// 入力方向ベクトルを作成
	XMVECTOR vec = SetInputDir(inputX, inputZ, player);

	// 電線ベクトルと入力方向ベクトルの内積を計算
	float dot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(lineVec), XMVector3Normalize(vec)));

	// 内積が負の場合、方向を反転
	if (dot < 0.0f)
	{
		// 目的地と開始地点を入れ替え
		std::swap(m_MoveCtx.StartPole, m_MoveCtx.DestPole);

		// パラメータtを反転
		m_MoveCtx.t = 1.0f - m_MoveCtx.t;
	}
}

void PlayerMovement::Move(const Player* player)
{
	// 次の電柱に到達したか判定
	if (m_MoveCtx.t >= 1.0f)
	{
		// 到達したら目的地の電柱を開始電柱に設定
		m_MoveCtx.StartPole = m_MoveCtx.DestPole;
		// 次の目的地の電柱を取得
		m_MoveCtx.DestPole = player->GetPoleManager()->GetDirectionalPole(m_MoveCtx.StartPole, m_MoveCtx.LastInputDir);
		// tをリセット
		m_MoveCtx.t = 0.0f;
		// 新しい電線IDを取得
		m_MoveCtx.LineID = player->GetPoleManager()->GetPowerLineID(m_MoveCtx.StartPole, m_MoveCtx.DestPole);
	}

	// tを進める
	float lineLength = player->GetPoleManager()->GetPowerLineLength(m_MoveCtx.LineID);
	m_MoveCtx.t += (m_MoveCtx.LineMoveSpeed / lineLength);

	// 電線上の位置を取得してプレイヤーの位置を更新
	XMFLOAT3 newPos = player->GetPoleManager()->GetPositionOnPowerLine(m_MoveCtx.StartPole, m_MoveCtx.DestPole, m_MoveCtx.t);
	SetMoveVec(newPos);
}

void PlayerMovement::Eject(float inputX, float inputZ, const Player* player)
{
	// 電線上からの射出処理
	ElectricJump(inputX, inputZ, player);
}

void PlayerMovement::ConvertLineSpeedToVelocity()
{
}


DirectX::XMFLOAT3 ConvertToWorldFromInput(const DirectX::XMFLOAT3 inputDir, const Camera* camera)
{
	return DirectX::XMFLOAT3();
}
