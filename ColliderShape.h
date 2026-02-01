#ifndef COLLIDERSHAPE_H
#define COLLIDERSHAPE_H

#include "Vector3.h"
#include "Quaternion.h"

class PhysicsBody;

// コライダー形状の種類のenum
enum class ShapeType
{
	BOX,
	SPHERE,
	CAPSULE,
	CYLINDER
};

// コライダー形状設定構造体
struct ColliderShapeDesc
{
	Vector3 OffsetPos{ 0.0f, 0.0f, 0.0f };
	Quaternion OffsetRot{ Quaternion::Identity() };
	float Friction = 0.0f;
	float Restitution = 0.0f;
};
// コライダー形状コンポーネントクラス
struct ColliderShape
{
	virtual ~ColliderShape() = default;

	// コライダー形状の種類
	const ShapeType Type = {};

	// サイズ 
	// BOXの場合は各辺の長さ
	// SPHEREの場合は半径
	// CAPSULEの場合は半径、高さ
	// CYLINDERの場合は半径、高さ
	const Vector3 Size = {};

	// オフセット位置・回転
	const Vector3 OffsetPos = {};
	const Quaternion OffsetRot = {};

	// 物理マテリアル
	const float Friction = 0.0f;
	const float Restitution = 0.0f;

protected:
	// 継承クラス用コンストラクタ
	ColliderShape(ShapeType type, const Vector3& size, const ColliderShapeDesc& desc)
		: Type(type), Size(size), OffsetPos(desc.OffsetPos), OffsetRot(desc.OffsetRot),
		  Friction(desc.Friction), Restitution(desc.Restitution)
	{}

	friend class PhysicsSystem;
};





// ボックスコライダー形状設定構造体
struct BoxColliderDesc : public ColliderShapeDesc
{
	float SizeX = 1.0f;
	float SizeY = 1.0f;
	float SizeZ = 1.0f;
};
// ボックスコライダー形状コンポーネントクラス
struct BoxColliderShape : public ColliderShape
{
	BoxColliderShape(const BoxColliderDesc& desc)
		: ColliderShape(ShapeType::BOX, Vector3(desc.SizeX, desc.SizeY, desc.SizeZ), desc)
	{}
};






// スフィアコライダー形状設定構造体
struct SphereColliderDesc : public ColliderShapeDesc
{
	float Radius = 1.0f;
};
// スフィアコライダー形状コンポーネントクラス
struct SphereColliderShape : public ColliderShape
{
	SphereColliderShape(const SphereColliderDesc& desc)
		: ColliderShape(ShapeType::SPHERE, Vector3(desc.Radius, 0.0f, 0.0f), desc)
	{}
};





// カプセルコライダー形状設定構造体
struct CapsuleColliderDesc : public ColliderShapeDesc
{
	float Radius = 0.5f;
	float Height = 1.0f;
};
// カプセルコライダー形状コンポーネントクラス
struct CapsuleColliderShape : public ColliderShape
{
	CapsuleColliderShape(const CapsuleColliderDesc& desc)
		: ColliderShape(ShapeType::CAPSULE, Vector3(desc.Radius, desc.Height, 0.0f), desc)
	{}
};




// シリンダーコライダー形状設定構造体
struct CylinderColliderDesc : public ColliderShapeDesc
{
	float Radius = 0.5f;
	float Height = 1.0f;
};
// シリンダーコライダー形状コンポーネントクラス
struct CylinderColliderShape : public ColliderShape
{
	CylinderColliderShape(const CylinderColliderDesc& desc)
		: ColliderShape(ShapeType::CYLINDER, Vector3(desc.Radius, desc.Height, 0.0f), desc)
	{}
};


#endif