#include "Transform.h"

using namespace DirectX;

Vector3& Transform::position()
{
	MarkDirty();
	return m_localPosition;
}

Quaternion& Transform::rotation()
{
	MarkDirty();
	return m_localRotation;
}

Vector3& Transform::scale()
{
	MarkDirty();
	return m_localScale;
}

void Transform::SetParent(Transform& parent)
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

void Transform::RemoveChild(Transform& child)
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

void Transform::RemoveParent()
{
	if (m_parent)
	{
		m_parent->RemoveChild(*this);
		m_parent = nullptr;
		MarkDirty();
	}
}

DirectX::XMMATRIX Transform::GetWorldMatrix() const
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

void Transform::SetLocalMatrix(const DirectX::XMMATRIX& localMatrix)
{
	// 各種成分を分解
	XMVECTOR scaleVec{};
	XMVECTOR rotationVec{};
	XMVECTOR translationVec{};
	XMMatrixDecompose(&scaleVec, &rotationVec, &translationVec, localMatrix);

	// 各成分を設定
	m_localScale.FromXMVECTOR(scaleVec);
	m_localRotation.FromXMVECTOR(rotationVec);
	m_localPosition.FromXMVECTOR(translationVec);
	MarkDirty();
}

DirectX::XMMATRIX Transform::GetLocalMatrix() const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_localPosition.x, m_localPosition.y, m_localPosition.z);
	DirectX::XMMATRIX rotationMatrix = m_localRotation.ToXMMATRIX();
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_localScale.x, m_localScale.y, m_localScale.z);
	DirectX::XMMATRIX localMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	return localMatrix;
}

void Transform::MarkDirty()
{
	isDirty = true;
	// 子供のTransformも更新が必要になる
	for (auto& child : m_children)
	{
		child->MarkDirty();
	}
}
