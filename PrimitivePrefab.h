#ifndef PRIMITIVE_PREFAB_H
#define PRIMITIVE_PREFAB_H

#include "Prefab.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "RenderingSystem.h"

class PrimitivePrefab : public Prefab
{
public:
	virtual ~PrimitivePrefab() = default;

	enum class PrimitiveType
	{
		Cube,
		Sphere,
		Plane,
		Capsule,
		Cylinder
	};

	// プリミティブタイプ
	PrimitiveType Type = PrimitiveType::Cube;

	Mesh* MeshPtr = nullptr;

	// テクスチャインデックス
	int TextureIndex = -1;

	void Instantiate(GameObject& gameObject) override
	{
		auto* renderer = gameObject.AddComponent<MeshRenderer>();

		switch (Type)
		{
		case PrimitivePrefab::PrimitiveType::Cube:
			MeshPtr = &RenderingSystem::cubeMesh;
			break;
		case PrimitivePrefab::PrimitiveType::Sphere:
			MeshPtr = &RenderingSystem::sphereMesh;
			break;
		case PrimitivePrefab::PrimitiveType::Plane:
			MeshPtr = &RenderingSystem::planeMesh;
			break;
		case PrimitivePrefab::PrimitiveType::Capsule:
			MeshPtr = &RenderingSystem::capsuleMesh;
			break;
		case PrimitivePrefab::PrimitiveType::Cylinder:
			MeshPtr = &RenderingSystem::cylinderMesh;
			break;
		default:
			break;
		}
		renderer->SetMesh(MeshPtr);
		renderer->SetTextureIndex(TextureIndex);
	}
};


class CubePrefab : public PrimitivePrefab
{
public:
	virtual ~CubePrefab() = default;

	void Instantiate(GameObject& gameObject) override
	{
		Type = PrimitiveType::Cube;
		PrimitivePrefab::Instantiate(gameObject);
	}
};

class SpherePrefab : public PrimitivePrefab
{
public:
	virtual ~SpherePrefab() = default;
	void Instantiate(GameObject& gameObject) override
	{
		Type = PrimitiveType::Sphere;
		PrimitivePrefab::Instantiate(gameObject);
	}
};

class PlanePrefab : public PrimitivePrefab
{
public:
	virtual ~PlanePrefab() = default;
	void Instantiate(GameObject& gameObject) override
	{
		Type = PrimitiveType::Plane;
		PrimitivePrefab::Instantiate(gameObject);
	}
};

class CapsulePrefab : public PrimitivePrefab
{
public:
	virtual ~CapsulePrefab() = default;
	void Instantiate(GameObject& gameObject) override
	{
		Type = PrimitiveType::Capsule;
		PrimitivePrefab::Instantiate(gameObject);
	}
};

class CylinderPrefab : public PrimitivePrefab
{
public:
	virtual ~CylinderPrefab() = default;
	void Instantiate(GameObject& gameObject) override
	{
		Type = PrimitiveType::Cylinder;
		PrimitivePrefab::Instantiate(gameObject);
	}
};

#endif
