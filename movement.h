

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <DirectXMath.h>
#include "GameObject.h"
#include "Component.h"
#include "RigidBody.h"

class Movement : public Component
{
private:
	// 速度ベクトル
	DirectX::XMFLOAT3 m_MoveVec{ 0.0f, 0.0f, 0.0f };
	// 力量ベクトル(毎フレームリセット)
	DirectX::XMFLOAT3 m_ForceVec{ 0.0f, 0.0f, 0.0f };

	// 移動適用用のRigidBodyポインタ
	RigidBody* m_RigidBody = nullptr;
public:
	Movement() = default;
	virtual ~Movement() = default;

	// 初期化処理
	virtual void Start() override;

	// 移動ベクトルの設定・取得
	void SetMoveVec(DirectX::XMFLOAT3 moveDir) { m_MoveVec = moveDir; }
	DirectX::XMFLOAT3 GetMoveVec() const { return m_MoveVec; }

	// 力量ベクトルの設定・取得
	void SetForceVec(DirectX::XMFLOAT3 forceVec) { m_ForceVec = forceVec; }
	DirectX::XMFLOAT3 GetForceVec() const { return m_ForceVec; }

	//　移動ベクトルの設定・取得（XMVECTOR版）
	void SetMoveVecV(DirectX::XMVECTOR moveDir) { DirectX::XMStoreFloat3(&m_MoveVec, moveDir); }
	DirectX::XMVECTOR GetMoveVecV() const { return DirectX::XMLoadFloat3(&m_MoveVec); }

	//　力量ベクトルの設定・取得（XMVECTOR版）
	void SetForceVecV(DirectX::XMVECTOR forceVec) { DirectX::XMStoreFloat3(&m_ForceVec, forceVec); }
	DirectX::XMVECTOR GetForceVecV() const { return DirectX::XMLoadFloat3(&m_ForceVec); }

	// 速度取得メソッド
	DirectX::XMFLOAT3 GetVelocityVec() { return m_RigidBody->GetVelocity(); }
	// 速度取得メソッド（XMVECTOR版）
	DirectX::XMVECTOR GetVelocityVecV() { return DirectX::XMLoadFloat3(&m_RigidBody->GetVelocity());}

	// 移動ベクトルの方向にオブジェクトを回転させる
	void RotateByMoveVec();

	// RigidBodyの速度をリセットするメソッド
	void ResetVelocity() { m_RigidBody->SetVelocity({ 0.0f, 0.0f, 0.0f }); }

	// 更新処理
	virtual void PostUpdate(double elapsedTime);
};

#endif
