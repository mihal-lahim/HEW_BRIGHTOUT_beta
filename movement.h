

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <DirectXMath.h>
#include "GameObject.h"
#include "Component.h"
#include "RigidBody.h"

class Movement : public Component
{
private:
	// 移動ベクトル（毎フレームリセット）
	DirectX::XMFLOAT3 m_MoveVec{ 0.0f, 0.0f, 0.0f };
	// 力量ベクトル(毎フレームリセット)
	DirectX::XMFLOAT3 m_ForceVec{ 0.0f, 0.0f, 0.0f };
	// 速度ベクトル
	DirectX::XMFLOAT3 m_VelocityVec{ 0.0f, 0.0f, 0.0f };

	// 移動適用用のRigidBodyポインタ
	RigidBody* m_RigidBody = nullptr;
public:
	Movement() = default;
	virtual ~Movement() = default;

	// 初期化処理
	virtual void Start() override;



	// 取得メソッド（XMFLOAT3版）
	DirectX::XMFLOAT3 GetMoveVec() const { return m_MoveVec; }
	DirectX::XMFLOAT3 GetForceVec() const { return m_ForceVec; }
	DirectX::XMFLOAT3 GetVelocityVec() const { return m_VelocityVec; }

	// 各種ベクトルの取得メソッド（XMVECTOR版）
	DirectX::XMVECTOR GetMoveVecV() const { return DirectX::XMLoadFloat3(&m_MoveVec); }
	DirectX::XMVECTOR GetForceVecV() const { return DirectX::XMLoadFloat3(&m_ForceVec); }
	DirectX::XMVECTOR GetVelocityVecV() const { return DirectX::XMLoadFloat3(&m_VelocityVec); }



	// 各種ベクトルの設定メソッド（XMFLOAT3版）
	void SetMoveVec(const DirectX::XMFLOAT3& moveVec) { m_MoveVec = moveVec; }
	void SetForceVec(const DirectX::XMFLOAT3& forceVec) { m_ForceVec = forceVec; }
	void SetVelocityVec(const DirectX::XMFLOAT3& velocityVec) { m_VelocityVec = velocityVec; }

	// 各種ベクトルの設定メソッド（XMVECTOR版）
	void SetMoveVecV(const DirectX::XMVECTOR& moveVec) { DirectX::XMStoreFloat3(&m_MoveVec, moveVec); }
	void SetForceVecV(const DirectX::XMVECTOR& forceVec) { DirectX::XMStoreFloat3(&m_ForceVec, forceVec); }
	void SetVelocityVecV(const DirectX::XMVECTOR& velocityVec) { DirectX::XMStoreFloat3(&m_VelocityVec, velocityVec); }



	// 各種ベクトルの加算メソッド（XMFLOAT3版）
	void AddMoveVec(const DirectX::XMFLOAT3& moveVec);
	void AddForceVec(const DirectX::XMFLOAT3& forceVec);
	void AddVelocityVec(const DirectX::XMFLOAT3& velocityVec);

	// 各種ベクトルの加算メソッド（XMVECTOR版）
	void AddMoveVecV(const DirectX::XMVECTOR& moveVec);
	void AddForceVecV(const DirectX::XMVECTOR& forceVec);
	void AddVelocityVecV(const DirectX::XMVECTOR& velocityVec);



	// RigidBodyの現在の速度を取得するメソッド（XMFLOAT3版）
	DirectX::XMFLOAT3 GetCurrentVelocity() const;

	// RigidBodyの現在の速度を取得するメソッド（XMVECTOR版）
	DirectX::XMVECTOR GetCurrentVelocityV() const;

	// 移動ベクトルの方向にオブジェクトを回転させる
	void RotateByMoveVec();

	// RigidBodyの速度をリセットするメソッド
	void ResetVelocity() { m_RigidBody->SetVelocity({ 0.0f, 0.0f, 0.0f }); }

	// 更新処理
	virtual void PostUpdate();
};

#endif
