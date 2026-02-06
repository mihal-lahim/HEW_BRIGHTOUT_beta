#ifndef PRIMITIVE_PREFAB_H
#define PRIMITIVE_PREFAB_H

#include "Prefab.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "ResourceSystem.h"
#include "Texture.h"
#include "Mesh.h"
#include "shader.h"

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

	// メッシュレンダラーとマテリアル
	MeshRenderer* meshRenderer = nullptr;

	void Instantiate(GameObject& gameObject) override
	{
		meshRenderer = gameObject.AddComponent<MeshRenderer>();

		//Mesh* mesh = nullptr;

		switch (type)
		{
		case PrimitivePrefab::PrimitiveType::Cube:
			//mesh = gameObject.resource().Load<CubeMesh<MeshVS>>();
			break;
		case PrimitivePrefab::PrimitiveType::Sphere:
			//mesh = gameObject.resource().Load<SphereMesh<MeshVS>>();
			break;
		case PrimitivePrefab::PrimitiveType::Plane:
			//mesh = gameObject.resource().Load<PlaneMesh<MeshVS>>();
			break;
		case PrimitivePrefab::PrimitiveType::Capsule:
			//mesh = gameObject.resource().Load<CapsuleMesh<MeshVS>>();
			break;
		case PrimitivePrefab::PrimitiveType::Cylinder:
			//mesh = gameObject.resource().Load<CylinderMesh<MeshVS>>();
			break;
		default:
			break;
		}

		//meshRenderer->mesh = mesh;
	}

protected:
	// プリミティブタイプ
	PrimitiveType type = PrimitiveType::Cube;
};


class CubePrefab : public PrimitivePrefab
{
public:
	virtual ~CubePrefab() = default;

	void Instantiate(GameObject& gameObject) override
	{
		type = PrimitiveType::Cube;
		PrimitivePrefab::Instantiate(gameObject);
	}
};

class SpherePrefab : public PrimitivePrefab
{
public:
	virtual ~SpherePrefab() = default;
	void Instantiate(GameObject& gameObject) override
	{
		type = PrimitiveType::Sphere;
		PrimitivePrefab::Instantiate(gameObject);
	}
};

class PlanePrefab : public PrimitivePrefab
{
public:
	virtual ~PlanePrefab() = default;
	void Instantiate(GameObject& gameObject) override
	{
		type = PrimitiveType::Plane;
		PrimitivePrefab::Instantiate(gameObject);
	}
};

class CapsulePrefab : public PrimitivePrefab
{
public:
	virtual ~CapsulePrefab() = default;
	void Instantiate(GameObject& gameObject) override
	{
		type = PrimitiveType::Capsule;
		PrimitivePrefab::Instantiate(gameObject);
	}
};

class CylinderPrefab : public PrimitivePrefab
{
public:
	virtual ~CylinderPrefab() = default;
	void Instantiate(GameObject& gameObject) override
	{
		type = PrimitiveType::Cylinder;
		PrimitivePrefab::Instantiate(gameObject);
	}
};

#endif
