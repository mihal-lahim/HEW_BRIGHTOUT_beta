

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vector3.h"
#include "Quaternion.h"
#include "Component.h"


class Transform : public Component
{
public:

	Transform() = default;
	~Transform() = default;

	// 位置の取得・設定メソッド
	Vector3& position() 
	{ 
		MarkDirty();
		return m_localPosition;
	}

	// 回転の取得・設定メソッド
	Quaternion& rotation() 
	{ 
		MarkDirty();
		return m_localRotation;
	}

	// スケールの取得・設定メソッド
	Vector3& scale() 
	{ 
		MarkDirty();
		return m_localScale;
	}

	// 親の設定
	void SetParent(Transform& parent)
	{
		// 既に親がいる場合は、現在の親から自分を削除
		if (m_parent)
		{
			m_parent->RemoveChild(*this);
		}

		// 新しい親を設定し、親の子リストに自分を追加
		m_parent = &parent;
		parent.m_children.push_back(this);
		MarkDirty();
	}

	// 子の追加
	void AddChild(Transform& child)
	{
		child.SetParent(*this);
	}

	// 子の削除
	void RemoveChild(Transform& child)
	{
		child.m_parent = nullptr;
		m_children.erase(
			std::remove(
				m_children.begin(),
				m_children.end(),
				&child),
			m_children.end()
		);
		MarkDirty();
	}

	// 親の削除
	void RemoveParent()
	{
		if (m_parent)
		{
			m_parent->RemoveChild(*this);
			m_parent = nullptr;
			MarkDirty();
		}
	}

	// 親の取得
	Transform* GetParent()
	{
		return m_parent;
	}

	// 子の取得
	std::vector<Transform*>& GetChildren()
	{
		return m_children;
	}

	// ワールド変換行列の取得メソッド
	DirectX::XMMATRIX GetWorldMatrix() const
	{
		if (isDirty)
		{
			// ローカル変換行列の計算
			DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_localPosition.x, m_localPosition.y, m_localPosition.z);
			DirectX::XMMATRIX rotationMatrix = m_localRotation.ToXMMATRIX();
			DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_localScale.x, m_localScale.y, m_localScale.z);
			DirectX::XMMATRIX localMatrix = scaleMatrix * rotationMatrix * translationMatrix;
			// 親の変換行列を考慮
			if (m_parent)
			{
				m_worldMatrix = localMatrix * m_parent->GetWorldMatrix();
			}
			else
			{
				m_worldMatrix = localMatrix;
			}
			isDirty = false;
		}
		return m_worldMatrix;
	}

	// ローカル変換行列の設定メソッド
	void SetLocalMatrix(const DirectX::XMMATRIX& localMatrix)
	{
		// スケールの抽出
		DirectX::XMVECTOR scaleVec;
		DirectX::XMVECTOR rotationVec;
		DirectX::XMVECTOR translationVec;
		DirectX::XMMatrixDecompose(&scaleVec, &rotationVec, &translationVec, localMatrix);
		// 各成分を設定
		m_localScale.FromXMVECTOR(scaleVec);
		m_localRotation.FromXMVECTOR(rotationVec);
		m_localPosition.FromXMVECTOR(translationVec);
		MarkDirty();
	}

private:

	void MarkDirty()
	{
		isDirty = true;
		for (auto* child : m_children)
		{
			child->MarkDirty();
		}
	}

	// 位置
	Vector3 m_localPosition{};
	// 回転（クオータニオン）
	Quaternion m_localRotation{};
	// スケール
	Vector3 m_localScale{ 1.0f, 1.0f, 1.0f };

	// キャッシュされた変換行列
	mutable DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();

	// 変換行列が更新されたかどうかのフラグ
	mutable bool isDirty = true;

	// 親子関係
	Transform* m_parent = nullptr;
	std::vector<Transform*> m_children = {};
};


#endif
