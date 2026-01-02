#include "player.h"
#include <cmath>
#include <DirectXMath.h>
#include "model.h"
#include "controller.h"
#include "top_down_camera.h"
#include "ObjectManager.h"
#include "Pole.h"
#include "PowerLine.h"
#include "debug_console.h"

// 入力反転フラグ（必要に応じて調整）
static constexpr bool INVERT_LS_X = true; // 左右が反転しているので X を反転
static constexpr bool INVERT_LS_Y = true; // 前後が反転しているので Y を反転

using namespace DirectX;



// 毎フレーム更新（ダッシュ継続時間の管理、入力処理）
void Player::Update(double elapsedSec)
{
	//ジャンプ
	if (controller_->WasPressed(Controller::BUTTON_A)) {
		Jump(15);
	}

	// カメラを更新（右スティックの入力処理を含む）
	if (camera_) {
		camera_->Update(elapsedSec);
	}

	// 電線ダメージ処理：電気状態の間、定期的にダメージを受ける
	if (state == State::ELECTRICITY) {
		powerLineDamageTimer_ -= static_cast<float>(elapsedSec);
		if (powerLineDamageTimer_ <= 0.0f) {
			TakeDamage(POWERLINE_DAMAGE_AMOUNT);
			powerLineDamageTimer_ = POWERLINE_DAMAGE_INTERVAL; // タイマーをリセット
		}
	}

	// 1. 入力に基づいて水平方向の移動ベクトルを決定
	XMFLOAT3 horizontalMove = { 0.0f, 0.0f, 0.0f };
	if (controller_) {
		// 電気ボタン (B ボタンで変身)
		if (controller_->WasPressed(Controller::BUTTON_B)) {
			if (state == State::HUMAN) {
				// HUMAN -> ELECTRICITY: 電柱近く必須
				if (IsNearPole()) {
					ChangeState(State::ELECTRICITY);
					// 電気状態へのリセット処理
					ResetToElectricityState();

					//電気化した瞬間は進行方向を初期化
					electricMoveDir_ = XMFLOAT3(0.0f, 0.0f, 0.0f);


					// ★ 電線に即スナップ
					SnapToNearestPowerLine();

				}
			} else {
				// ELECTRICITY -> HUMAN: 電柱近く必須
				if (IsNearPole()) {
					ChangeState(State::HUMAN);
					/*if (!isDashing_) {
					currentSpeed_ = baseSpeed_;
					}*/
					// 電柱から跳ね返す処理
					KnockbackFromPole();
					// 0.5秒間、衝突判定をスキップ
					skipCollisionTimer_ = SKIP_COLLISION_DURATION;

					//人間に戻る時もリセット
					//electricMoveDir_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
				}
			}
		}

		// スティック入力
		auto left = controller_->GetStick(Controller::DIR_LEFT);
		if (left.outside) {
			float cameraAngle = camera_ ? camera_->GetCameraAngle() : 0.0f;
			float stickX = static_cast<float>(left.x) * (INVERT_LS_X ? -1.0f : 1.0f);
			float stickY = static_cast<float>(left.y) * (INVERT_LS_Y ? -1.0f : 1.0f);

			XMVECTOR camForward = XMVectorSet(sinf(cameraAngle), 0.0f, cosf(cameraAngle), 0.0f);
			XMVECTOR camRight = XMVectorSet(cosf(cameraAngle), 0.0f, -sinf(cameraAngle), 0.0f);
			XMVECTOR moveVec = XMVector3Normalize(XMVectorAdd(XMVectorScale(camRight, stickX), XMVectorScale(camForward, stickY)));
			XMStoreFloat3(&horizontalMove, moveVec);

			////電気状態なら進行方向を保存
			//if (state == State::ELECTRICITY) {
			//	electricMoveDir_ = horizontalMove;
			//}

			if (state == State::ELECTRICITY) {
				if (horizontalMove.x != 0.0f || horizontalMove.z != 0.0f) {
					electricMoveDir_ = horizontalMove;
				}
			}
		}

		//入力がない場合でも電線上では進み続ける
		if (state == State::ELECTRICITY) {
			horizontalMove = electricMoveDir_;
		}

        // デバッグ用ダメージ
		if (controller_->WasPressed(Controller::BUTTON_RIGHT_SHOULDER)) {
			TakeDamage(10);
		}
		//死亡用判定
		if(!IsAlive())
		{
			usePlayer = false;
		}
		//リスポーン処理
		if (usePlayer == false)
		{
			health_ = maxHealth_;
			m_Position = XMFLOAT3(0.0f, 15.0f, 0.0f);
			usePlayer = true;
			
			// 状態をHUMANにリセット
			ChangeState(State::HUMAN);
			
			// 移動速度を設定
			currentSpeed_ = baseSpeed_;
			
			// ダッシュ状態をリセット
			//isDashing_ = false;
			//dashTimeRemaining_ = 0.0f;
			
			// その他の状態をリセット
			velocityY_ = 0.0f;
			isGrounded_ = false;
			powerLineDamageTimer_ = 0.0f;
			skipCollisionTimer_ = 0.0f;

			electricMoveDir_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
		
	}

	//// 2. 重力を適用（毎フレーム速度を加算）
	if (!isGrounded_) {
		velocityY_ -= GRAVITY * static_cast<float>(elapsedSec);
	}

	if (state == State::HUMAN)
	{
		if (!isGrounded_)
		{
			velocityY_ -= GRAVITY * static_cast<float>(elapsedSec);
		}
	}
	else if (state == State::ELECTRICITY)
	{
		// 電気状態では重力無効・地面無視
		velocityY_ = 0.0f;
		isGrounded_ = true;
	}


	// 3. 移動と衝突判定
	XMFLOAT3 desiredMove = {
		horizontalMove.x * currentSpeed_ * static_cast<float>(elapsedSec),
		velocityY_ * static_cast<float>(elapsedSec),
		horizontalMove.z * currentSpeed_ * static_cast<float>(elapsedSec)
	};
	
	// 電気状態の場合は衝突判定をスキップして直進
	if (state == State::ELECTRICITY) {
		m_Position.x += desiredMove.x;
		m_Position.z += desiredMove.z;
		velocityY_ = 0.0f;
		isGrounded_ = true;
	} else if (skipCollisionTimer_ > 0.0f) {
		// 状態変化直後のスキップタイマー期間、衝突判定をスキップ
		m_Position.x += desiredMove.x;
		m_Position.z += desiredMove.z;
		m_Position.y += desiredMove.y;
	} else {
		// HUMAN状態は通常の衝突判定を適用
		ResolveCollisions(desiredMove);
	}

	// 4. 電気状態の場合、電線にスナップさせて落下防止
	if (state == State::ELECTRICITY) {
		SnapToNearestPowerLine();
	}

	// 5. 向きの更新
	if (horizontalMove.x != 0.0f || horizontalMove.z != 0.0f) {
		direction_ = horizontalMove;
	}

	//地面に設置したら速度を通常通りに戻す
	if (state == State::HUMAN) {
		// 空中 → 地面 に変わった瞬間
		if (isGrounded_) {
			currentSpeed_ = baseSpeed_;
		}
	}


	if (state == State::ELECTRICITY)
	{
		// 毎フレーム最寄りの電線を更新
		SnapToNearestPowerLine();
		if (!currentPowerLine_) return;

		XMFLOAT3 a = currentPowerLine_->GetStartPosition();
		XMFLOAT3 b = currentPowerLine_->GetEndPosition();

		XMFLOAT3 dir{
			b.x - a.x,
			b.y - a.y,
			b.z - a.z
		};

		float len = sqrtf(dir.x * dir.x + dir.z * dir.z);
		if (len > 0.001f) {
			dir.x /= len;
			dir.z /= len;
		}

		// 入力方向に合わせて向きを反転
		float dot =
			dir.x * electricMoveDir_.x +
			dir.z * electricMoveDir_.z;

		if (dot < 0.0f) {
			dir.x = -dir.x;
			dir.z = -dir.z;
		}

		// 電線方向に進む
		m_Position.x += dir.x * currentSpeed_ * static_cast<float>(elapsedSec);
		m_Position.z += dir.z * currentSpeed_ * static_cast<float>(elapsedSec);

		// 最終スナップ（角対策）
		SnapToNearestPowerLine();
	}

}

// 衝突解決
void Player::ResolveCollisions(DirectX::XMFLOAT3& desiredMove/*, double elapsedSec*/)
{
    // XZ平面（水平）の移動と衝突判定
    XMFLOAT3 newPos = m_Position;
    newPos.x += desiredMove.x;
    newPos.z += desiredMove.z;

    AABB playerAABB = GetAABBAt(newPos);
    bool collisionXZ = false;

    extern ObjectManager g_ObjectManager;
    for (const auto& obj : g_ObjectManager.GetGameObjects()) {
        // HUMAN状態の際、PowerLineとの衝突をスキップ
        if (state == State::HUMAN && obj->GetTag() == GameObjectTag::POWER_LINE) {
            continue;
        }
        // プレイヤー自身は衝突判定から除外
        if (obj->GetTag() == GameObjectTag::PLAYER) {
            continue;
        }
        
        if (playerAABB.IsOverlap(obj->GetAABB())) {
            collisionXZ = true;
            break;
        }
    }

    if (!collisionXZ) {
        m_Position = newPos; // 衝突なければ移動を適用
    }

    // Y軸（垂直）の移動と衝突判定
    m_Position.y += desiredMove.y;
    playerAABB = GetAABB(); // 更新されたXZ位置でAABBを再計算
    bool collisionY = false;

    for (const auto& obj : g_ObjectManager.GetGameObjects()) {
        // HUMAN状態の際、PowerLineとの衝突をスキップ
        if (state == State::HUMAN && obj->GetTag() == GameObjectTag::POWER_LINE) {
            continue;
        }
        // プレイヤー自身は衝突判定から除外
        if (obj->GetTag() == GameObjectTag::PLAYER) {
            continue;
        }
        
        if (playerAABB.IsOverlap(obj->GetAABB())) {
            // オブジェクトの上にあるか、下にあるかで分ける
            if (desiredMove.y < 0.0f) { // 落下時
                m_Position.y = obj->GetAABB().GetMax().y; // オブジェクトの上面にスナップ
                velocityY_ = 0.0f;
                isGrounded_ = true;

				// 着地したらリセット
				canAirJump_ = false;
            } else if (desiredMove.y > 0.0f) { // 上昇時
                m_Position.y = obj->GetAABB().GetMin().y - aabbHalfSize.y * 2.0f; // オブジェクトの下面にスナップ
                velocityY_ = 0.0f;
            }
            collisionY = true;
            break;
        }
    }

    // 地面との最終チェック
    if (m_Position.y <= GROUND_LEVEL) {
        m_Position.y = GROUND_LEVEL;
        velocityY_ = 0.0f;
        isGrounded_ = true;
    } else if (!collisionY) {
        isGrounded_ = false;
    }
}

// 指定した位置でのAABBを計算する補助関数
AABB Player::GetAABBAt(const DirectX::XMFLOAT3& pos) const
{
    float halfW = aabbHalfSize.x;
    float height = aabbHalfSize.y * 2.0f; // AABBの高さは全て
    DirectX::XMFLOAT3 min{ pos.x - halfW, pos.y - 0.5f, pos.z - halfW };
    DirectX::XMFLOAT3 max{ pos.x + halfW, pos.y + height, pos.z + halfW };
    return AABB(min, max);
}

// プレイヤー描画（状態に応じてモデルを選択）
void Player::Draw() const
{
	MODEL* drawModel = (state == State::ELECTRICITY && electricModel_) ? electricModel_ : model_;
	if (!drawModel) return;
	// 回転とY軸のみ向きの値で作成
	float yaw = atan2f(direction_.x, direction_.z);
	XMMATRIX matRot = XMMatrixRotationY(yaw);
	XMMATRIX matTrans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	XMMATRIX world = matRot * matTrans;

	ModelDraw(drawModel, world);
}


void Player::SetController(Controller* controller)
{
	controller_ = controller;
}

Controller* Player::GetController() const
{
	return controller_;
}

// カメラ生成（高さは target.y を利用）
void Player::CreateCamera(const XMFLOAT3& target)
{
	float height = target.y;
	camera_.reset(new TopDownCamera(this, height, 20.0f));
}

Camera* Player::GetCamera() const
{
	return camera_.get();
}

AABB Player::GetAABB() const
{
	float halfW = aabbHalfSize.x;
	float height = aabbHalfSize.y;
	DirectX::XMFLOAT3 min{ m_Position.x - halfW, m_Position.y, m_Position.z - halfW };
	DirectX::XMFLOAT3 max{ m_Position.x + halfW, m_Position.y + height, m_Position.z + halfW };
	return AABB(min, max);
}

// 電柱周辺検出メソッド
bool Player::IsNearPole() const
{
	extern ObjectManager g_ObjectManager;
	const auto& allObjects = g_ObjectManager.GetGameObjects();
	
	for (const auto& obj : allObjects) {
		if (obj->GetTag() == GameObjectTag::POLE) {
			Pole* pole = static_cast<Pole*>(obj.get());
			if (!pole) continue;
			
			DirectX::XMFLOAT3 polePos = pole->GetPosition();
			float dx = polePos.x - m_Position.x;
			float dz = polePos.z - m_Position.z;
			// 水平距離のみで判定（垂直は無視）
			float horizontalDistance = sqrtf(dx*dx + dz*dz);
			
			if (horizontalDistance <= POLE_DETECTION_RADIUS) {
				return true;
			}
		}
	}
	return false;
}

void Player::SnapToNearestPowerLine()
{
	// 電気状態でなければ何もしない
	if (state != State::ELECTRICITY) return;

	// ゲーム内の全オブジェクト管理クラス（外部参照）
	extern ObjectManager g_ObjectManager;

	// 最も近い電線との距離（初期値は最大）
	float minDistance = FLT_MAX;

	// スナップ後のプレイヤー位置
	XMFLOAT3 snappedPos = m_Position;

	// 最終的に選ばれた電線
	PowerLine* nearestLine = nullptr;

	// ============================================
	// 全ての電線オブジェクトを探索
	// ============================================
	for (const auto& obj : g_ObjectManager.GetGameObjects())
	{
		// 電線以外は無視
		if (obj->GetTag() != GameObjectTag::POWER_LINE) continue;

		// 電線オブジェクトにキャスト
		PowerLine* line = static_cast<PowerLine*>(obj.get());
		if (!line) continue;

		// プレイヤー位置から電線への最近接点を取得
		XMFLOAT3 closest = line->GetClosestPointOnLine(m_Position);

		// 最近接点との差分
		float dx = closest.x - m_Position.x;
		float dy = closest.y - m_Position.y;
		float dz = closest.z - m_Position.z;

		// XZ平面上での距離（電線に「乗れる」かの判定）
		float horizontalDist = sqrtf(dx * dx + dz * dz);

		// 高さ方向の距離（別階層の電線誤検出防止）
		float verticalDist = fabsf(dy);

		// ============================================
		// スナップ可能距離内か判定
		// ============================================
		if (horizontalDist <= POWER_LINE_SNAP_DISTANCE && verticalDist <= 5.0f)
		{
			// 3次元距離で最も近い電線を選択
			float dist = sqrtf(dx * dx + dy * dy + dz * dz);

			if (dist < minDistance)
			{
				// 最短距離を更新
				minDistance = dist;

				// スナップ位置を更新
				snappedPos = closest;

				// 最も近い電線として記録
				nearestLine = line;
			}
		}
	}

	if (nearestLine)
	{
		// プレイヤー位置を電線上にスナップ
		m_Position = snappedPos;
		// 現在乗っている電線を更新
		currentPowerLine_ = nearestLine;
		// 電線移動中は落下させない
		velocityY_ = 0.0f;
		isGrounded_ = true;
	}
}
// 電気状態から人間状態に変化する際に電柱から跳ね返す
void Player::KnockbackFromPole()
{
	extern ObjectManager g_ObjectManager;
	const auto& allObjects = g_ObjectManager.GetGameObjects();
	
	Pole* nearestPole = nullptr;
	float minDistance = FLT_MAX;
	
	for (const auto& obj : allObjects) {
		if (obj->GetTag() == GameObjectTag::POLE) {
			Pole* pole = static_cast<Pole*>(obj.get());
			if (!pole) continue;

			DirectX::XMFLOAT3 polePos = pole->GetPosition();
			float dx = polePos.x - m_Position.x;
			float dz = polePos.z - m_Position.z;
			float horizontalDistance = sqrtf(dx * dx + dz * dz);

			if (horizontalDistance < minDistance) {
				minDistance = horizontalDistance;
				nearestPole = pole;
			}
		}
	}

	if (!nearestPole) return;

	// 最も近い電柱からプレイヤーへの方向ベクトルを計算
	DirectX::XMFLOAT3 nearestPolePos = nearestPole->GetPosition();
	float knockbackDx = m_Position.x - nearestPolePos.x;
	float knockbackDz = m_Position.z - nearestPolePos.z;
	float knockbackDist = sqrtf(knockbackDx * knockbackDx + knockbackDz * knockbackDz);

	if (knockbackDist > 0.001f) {
		// 正規化して方向を計算
		knockbackDx /= knockbackDist;
		knockbackDz /= knockbackDist;

		// 電柱から確定に跳ね返す（KNOCKBACK_DISTANCE = 3.0f）
		m_Position.x = nearestPolePos.x + knockbackDx * KNOCKBACK_DISTANCE;
		m_Position.z = nearestPolePos.z + knockbackDz * KNOCKBACK_DISTANCE;

		// 同時にジャンプ（水平方向の移動なし）
		velocityY_ = KNOCKBACK_JUMP_FORCE;
		isGrounded_ = false;
	}

	// 空中ジャンプを1回だけ許可
	canAirJump_ = true;
}

// 電気状態への変化時に各種状態をリセット
void Player::ResetToElectricityState()
{
	// 速度をリセット
	velocityY_ = 0.0f;
	isGrounded_ = true;
	isDashing_ = false;
	//dashTimeRemaining_ = 0.0f;

	// 電気状態の速度を設定
	currentSpeed_ = baseSpeed_ * electricSpeedmul;

	// ダメージタイマーをリセット：電気状態に変わった直後はダメージを受けないようにする
	powerLineDamageTimer_ = POWERLINE_DAMAGE_INTERVAL;

	// 最寄りの電線にスナップ
	SnapToNearestPowerLine();
}

// プレイヤーステート変更メソッド
void Player::ChangeState(Player::State newState)
{
	if (state == newState) return; // 状態が変わらない場合は処理しない

	state = newState;

	// HUMAN状態に戻る場合はタイマーをリセット
	if (newState == State::HUMAN) {
		powerLineDamageTimer_ = 0.0f;
		DEBUG_LOGF("[ChangeState] HUMAN | Pos=(%.1f, %.1f, %.1f) | HP=%.1f/%.1f", 
			m_Position.x, m_Position.y, m_Position.z, health_, maxHealth_);
	} else if (newState == State::ELECTRICITY) {
		DEBUG_LOGF("[ChangeState] ELECTRICITY | Pos=(%.1f, %.1f, %.1f) | HP=%.1f/%.1f", 
			m_Position.x, m_Position.y, m_Position.z, health_, maxHealth_);
	}
}
