#include "PlayerMovement.h"
#include "player.h"
#include "PowerLine.h"
#include <algorithm>
#include "Camera.h"
#include "ObjectManager.h"
#include "Ray.h"


Vector3 PlayerMovement::SetInputDir(float inputX, float inputZ)
{
	// 入力方向ベクトルを作成
	Vector3 inputVec{ inputX, 0.0f, inputZ };

	// 入力方向をカメラの向きに合わせて変換
	Vector3 convertedVec = ConvertToWorldFromInput(inputVec, m_Camera);

	// 最後の入力方向を保存
	m_LastInputDir = convertedVec;

	return convertedVec;
}

bool PlayerMovement::IsOnGround() const
{
	if (!m_GroundRay.IsHit || m_GroundRay.HitDistance > m_Ctx.GroundDetectOffset)
		return false;

	return true;
}

void PlayerMovement::UpdateRayCast()
{
	// レイの始点を設定
	Vector3 from = gameObject()->transform.Position;
	from.y -= m_Ctx.RayCastOffset;

	// レイの作成
	m_GroundRay = Ray(from, { 0.0f, -1.0f, 0.0f });

	// レイキャスト実行
	m_GroundRay.RayCast(m_Ctx.RayLength);
}

void PlayerMovement::ApplyGravity()
{
	ForceVec.y += m_Ctx.Gravity;
}

void PlayerMovement::GroundMove(float inputX, float inputZ, float speed)
{
	// 入力方向ベクトルを作成
	Vector3 vec = ConvertToXZPlane(SetInputDir(inputX, inputZ));

	// 移動方向がゼロベクトルでなければ移動ベクトルを更新
	if (!vec.IsZero())
		// 新しい移動ベクトルを加算
		MoveVec += vec * speed;
}

void PlayerMovement::Walk(float inputX, float inputZ)
{
	GroundMove(inputX, inputZ, m_Ctx.WalkSpeed);
}

void PlayerMovement::Run(float inputX, float inputZ)
{
	GroundMove(inputX, inputZ, m_Ctx.WalkSpeed * 2.0f);
}


void PlayerMovement::Jump(float inputX, float inputZ, float force)
{
	// 入力方向ベクトルを作成
	Vector3 inputDir = ConvertToXZPlane(SetInputDir(inputX, inputZ));

	ImpulseVec += inputDir * force + Vector3{ 0.0f, force, 0.0f };
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

void PlayerMovement::SnapToPowerLine(PowerLineID lineID)
{
	// 電線を取得
	PowerLine* powerLine = m_PoleManager->GetPowerLine(lineID);

	// 電線ベクトルを取得
	Vector3 lineVec = powerLine->GetLineVector();

	// 現在の速度ベクトルを取得
	Vector3 vec = VelocityVec;

	// 速度ベクトルがゼロベクトルの場合
	if (vec.IsZero())
	{
		// 移動ベクトルを使用
		vec = MoveVec;

		// 移動ベクトルもゼロベクトルの場合
		if (MoveVec.IsZero())
		{
			// カメラ方向を設定
			vec = ConvertToWorldFromInput({ 0.0f, 0.0f, 1.0f }, m_Camera);
		}
	}


	// 速度ベクトルと電線ベクトルの内積を計算
	float dot = lineVec.Normalize().Dot(vec.Normalize());

	// 内積の符号に基づいて開始電柱と目的地電柱を設定
	m_StartPole = dot >= 0.0f ? powerLine->GetPoles().first : powerLine->GetPoles().second;
	m_DestPole = dot >= 0.0f ? powerLine->GetPoles().second : powerLine->GetPoles().first;

	// 電線IDを設定
	m_LineID = lineID;


	// 電線ベクトルの半分の長さを取得
	Vector3 halfVec = lineVec * 0.5f;

	// 電線の中央位置を取得
	Vector3 lineMidPos = powerLine->transform.Position;

	// 開始位置を設定（内積の符号に基づく）
	Vector3 startPos = dot >= 0.0f ? lineMidPos - halfVec : lineMidPos + halfVec;

	// プレイヤーの現在位置を取得
	Vector3 playerPos = gameObject()->transform.Position;

	// 開始位置からプレイヤー位置へのベクトルを取得
	Vector3 toPlayerVec = playerPos - startPos;
	
	// 電線ベクトルの長さを取得
	float lineLength = powerLine->GetLength();

	// 電線上の位置パラメータtを設定（0.0f ~ 1.0fの範囲にクランプ）
	m_LineParam = std::clamp(toPlayerVec.Dot(lineVec) / lineLength, 0.0f, 1.0f);

	// 速度ベクトルの長さを取得
	float velocityLength = vec.Length();

	// 電線上速度を設定
	m_Ctx.LineMoveSpeed = velocityLength >= m_Ctx.LineMoveSpeedMin ? velocityLength : m_Ctx.LineMoveSpeedMin;
}

void PlayerMovement::Turn(float inputX, float inputZ)
{
	// 現在の電線を取得
	PowerLine* powerLine = m_PoleManager->GetPowerLine(m_LineID);

	// 接続されている電柱IDペアを取得
	std::pair<PoleID, PoleID> poles = powerLine->GetPoles();

	// 電線ベクトルを取得
	Vector3 lineVec = m_StartPole == poles.first ? powerLine->GetLineVector() : powerLine->GetLineVector() * -1.0f;

	// 入力方向ベクトルを作成
	Vector3 vec = SetInputDir(inputX, inputZ);

	// 電線ベクトルと入力方向ベクトルの内積を計算
	float dot = lineVec.Normalize().Dot(vec.Normalize());

	// 内積が負の場合、方向を反転
	if (dot < 0.0f)
	{
		// 目的地と開始地点を入れ替え
		std::swap(m_StartPole, m_DestPole);

		// パラメータtを反転
		m_LineParam = 1.0f - m_LineParam;
	}
}

void PlayerMovement::LineMove()
{
	// 次の電柱に到達したか判定
	if (m_LineParam >= 1.0f)
	{
		// 到達したら目的地の電柱を開始電柱に設定
		m_StartPole = m_DestPole;
		// 次の目的地の電柱を取得
		m_DestPole = m_PoleManager->GetDirectionalPole(m_StartPole, m_LastInputDir);
		// tをリセット
		m_LineParam = 0.0f;
		// 新しい電線IDを取得
		m_LineID = m_PoleManager->GetPowerLineID(m_StartPole, m_DestPole);
	}

	// tを進める
	float lineLength = m_PoleManager->GetPowerLineLength(m_LineID);
	m_LineParam += (m_Ctx.LineMoveSpeed / lineLength);

	// 電線上の位置を取得
	Vector3 newPos;
	newPos.FromXMFLOAT3(m_PoleManager->GetPositionOnPowerLine(m_StartPole, m_DestPole, m_LineParam));

	// 速度ベクトルを計算
	Vector3 currentPos = gameObject()->transform.Position;
	Vector3 velocityVec = newPos - currentPos;

	// 速度ベクトルを設定
	VelocityVec = velocityVec;
}

void PlayerMovement::Eject(float inputX, float inputZ)
{
	// 電線上からの射出処理
	ElectricJump(inputX, inputZ);
}

void PlayerMovement::PostUpdate()
{
	// レイキャストの更新
	UpdateRayCast();
	// 基底クラスの更新処理を呼び出し
	Movement::PostUpdate();
}


Vector3 ConvertToWorldFromInput(const Vector3& inputDir, const Camera* camera)
{
	// 入力方向ベクトルを回転させてワールド座標系に変換
	return inputDir.Rotate(camera->transform.Rotation);
}

Vector3 ConvertToXZPlane(const Vector3& worldDir)
{
	// ゼロベクトルの場合はそのまま返す
	if (worldDir.IsZero())
		return Vector3(0.0f, 0.0f, 0.0f);


	// 元のベクトルの長さを保存
	float originalLength = worldDir.Length();
	

	// Y成分を0にしてXZ平面に投影
	Vector3 xzDir = worldDir;
	xzDir.y = 0.0f;
	

	// XZ成分がゼロの場合（Y軸に平行な場合）はゼロベクトルを返す
	if (xzDir.IsZero())
		return Vector3(0.0f, 0.0f, 0.0f);



	// 元の長さを保持したままXZ平面上に投影
	return xzDir.Normalize() * originalLength;
}
