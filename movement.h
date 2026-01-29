

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <DirectXMath.h>
#include "GameObject.h"
#include "Component.h"
#include "RigidBody.h"

class Movement : public Component
{
private:
	// 移動適用用のRigidBodyポインタ
	RigidBody* m_RigidBody = nullptr;
public:

	// 移動ベクトル（毎フレームリセット）
	Vector3 MoveVec{ 0.0f, 0.0f, 0.0f };
	// 力量ベクトル(毎フレームリセット)
	Vector3 ForceVec{ 0.0f, 0.0f, 0.0f };
	// インパルスベクトル(毎フレームリセット)
	Vector3 ImpulseVec{ 0.0f, 0.0f, 0.0f };
	// 速度ベクトル
	Vector3 VelocityVec{ 0.0f, 0.0f, 0.0f };


	Movement() = default;
	virtual ~Movement() = default;

	// 初期化処理
	virtual void Start() override;


	// RigidBodyの現在の速度を取得するメソッド
	Vector3 GetCurrentVelocity() const;


	// 移動ベクトルの方向にオブジェクトを回転させる
	void RotateByMoveVec();

	// 更新処理
	virtual void PostUpdate();
};

#endif
