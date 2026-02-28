

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vector3.h"
#include "Quaternion.h"
#include "Component.h"
#include "include/DebugTest/DebugOstream.h"


class Transform : public Component
{
public:

	Transform() = default;
	~Transform() = default;

	// 位置の取得・設定メソッド
	Vector3& position();

	// 回転の取得・設定メソッド
	Quaternion& rotation();

	// スケールの取得・設定メソッド
	Vector3& scale();

	// 親の設定
	void SetParent(Transform& parent);

	// 子の追加
	void AddChild(Transform& child)
	{
		child.SetParent(*this);
	}

	// 子の削除
	void RemoveChild(Transform& child);

	// 親の削除
	void RemoveParent();

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
	DirectX::XMMATRIX GetWorldMatrix() const;

	// ローカル変換行列の設定メソッド
	void SetLocalMatrix(const DirectX::XMMATRIX& localMatrix);

	// ローカル変換行列の取得メソッド
	DirectX::XMMATRIX GetLocalMatrix() const;

private:

	// 変換行列が変更されたことをマークするメソッド
	void MarkDirty();

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
