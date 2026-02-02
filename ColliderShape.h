#ifndef COLLIDERSHAPE_H
#define COLLIDERSHAPE_H

#include "Vector3.h"
#include "Quaternion.h"
#include "Component.h"

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

// ボックスコライダー形状設定構造体
struct BoxColliderDesc : public ColliderShapeDesc
{
	float SizeX = 1.0f;
	float SizeY = 1.0f;
	float SizeZ = 1.0f;
};

// スフィアコライダー形状設定構造体
struct SphereColliderDesc : public ColliderShapeDesc
{
	float Radius = 1.0f;
};

// カプセルコライダー形状設定構造体
struct CapsuleColliderDesc : public ColliderShapeDesc
{
	float Radius = 0.5f;
	float Height = 1.0f;
};

// シリンダーコライダー形状設定構造体
struct CylinderColliderDesc : public ColliderShapeDesc
{
	float Radius = 0.5f;
	float Height = 1.0f;
};


// コライダー形状コンポーネントクラス
class ColliderShape : public Component
{
public:
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


	ColliderShape(const BoxColliderDesc& desc)
		: Type(ShapeType::BOX),
		Size(Vector3(desc.SizeX, desc.SizeY, desc.SizeZ)),
		OffsetPos(desc.OffsetPos),
		OffsetRot(desc.OffsetRot),
		Friction(desc.Friction),
		Restitution(desc.Restitution)
	{
	}

	ColliderShape(const SphereColliderDesc& desc)
		: Type(ShapeType::SPHERE),
		Size(Vector3(desc.Radius, 0.0f, 0.0f)),
		OffsetPos(desc.OffsetPos),
		OffsetRot(desc.OffsetRot),
		Friction(desc.Friction),
		Restitution(desc.Restitution)
	{
	}

	ColliderShape(const CapsuleColliderDesc& desc)
		: Type(ShapeType::CAPSULE),
		Size(Vector3(desc.Radius, desc.Height, 0.0f)),
		OffsetPos(desc.OffsetPos),
		OffsetRot(desc.OffsetRot),
		Friction(desc.Friction),
		Restitution(desc.Restitution)
	{
	}

	ColliderShape(const CylinderColliderDesc& desc)
		: Type(ShapeType::CYLINDER),
		Size(Vector3(desc.Radius, desc.Height, 0.0f)),
		OffsetPos(desc.OffsetPos),
		OffsetRot(desc.OffsetRot),
		Friction(desc.Friction),
		Restitution(desc.Restitution)
	{
	}

	friend class PhysicsSystem;
};


#endif