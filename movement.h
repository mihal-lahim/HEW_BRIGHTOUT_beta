

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <DirectXMath.h>

class GameObject;


class Movement
{
private:
	// 移動ベクトル(毎フレームリセット)
	DirectX::XMFLOAT3 m_MoveVec{ 0.0f, 0.0f, 0.0f };
	// 速度ベクトル（毎フレーム継続）
	DirectX::XMFLOAT3 m_VelocityVec{ 0.0f, 0.0f, 0.0f };
public:
	Movement() = default;
	virtual ~Movement() = default;

	// 移動ベクトルの設定・取得
	void SetMoveVec(DirectX::XMFLOAT3 moveDir) { m_MoveVec = moveDir; }
	DirectX::XMFLOAT3 GetMoveVec() const { return m_MoveVec; }

	// 速度ベクトルの設定・取得
	void SetVelocityVec(DirectX::XMFLOAT3 velocityVec) { m_VelocityVec = velocityVec; }
	DirectX::XMFLOAT3 GetVelocityVec() const { return m_VelocityVec; }

	//　移動ベクトルの設定・取得（XMVECTOR版）
	void SetMoveVec(DirectX::XMVECTOR moveDir) { DirectX::XMStoreFloat3(&m_MoveVec, moveDir); }
	DirectX::XMVECTOR GetMoveVecV() const { return DirectX::XMLoadFloat3(&m_MoveVec); }

	//　速度ベクトルの設定・取得（XMVECTOR版）
	void SetVelocityVec(DirectX::XMVECTOR velocityVec) { DirectX::XMStoreFloat3(&m_VelocityVec, velocityVec); }
	DirectX::XMVECTOR GetVelocityVecV() const { return DirectX::XMLoadFloat3(&m_VelocityVec); }

	// 更新処理
	virtual void Update(double elapsedTime, GameObject* owner);
};

#endif
