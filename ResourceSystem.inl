#ifndef RESOURCE_SYSTEM_INL
#define RESOURCE_SYSTEM_INL

#include "ResourceSystem.h"
#include "Mesh.h"
#include "Texture.h"

template<typename T>
	requires std::is_base_of_v<Resource, T>
inline T* ResourceSystem::LoadPrimitiveMesh(const std::wstring& key)
{
	if (m_resourceMap.count(key))
	{
		m_resourceMap[key].RefCount++;
		return static_cast<T*>(m_resourceMap[key].Resource.get());
	}

	auto& device = GetGraphicsDevice();

	auto resource = std::make_unique<T>();
	resource->m_filePath = key;
	resource->m_resourceSystem = this;
	resource->Initialize(device);
	T* ptr = resource.get();
	ResourceContainer container;
	container.RefCount = 1;
	container.Resource = std::move(resource);
	m_resourceMap[key] = std::move(container);
	return ptr;
}

template<>
inline CubeMesh* ResourceSystem::Load()
{
	return LoadPrimitiveMesh<CubeMesh>(L"CubeMesh");
}

template<>
inline PlaneMesh* ResourceSystem::Load()
{
	return LoadPrimitiveMesh<PlaneMesh>(L"PlaneMesh");
}

template<>
inline SphereMesh* ResourceSystem::Load()
{
	return LoadPrimitiveMesh<SphereMesh>(L"SphereMesh");
}

template<>
inline CapsuleMesh* ResourceSystem::Load()
{
	return LoadPrimitiveMesh<CapsuleMesh>(L"CapsuleMesh");
}

template<>
inline CylinderMesh* ResourceSystem::Load()
{
	return LoadPrimitiveMesh<CylinderMesh>(L"CylinderMesh");
}

template<>
inline Texture* ResourceSystem::Load(const std::wstring& filePath)
{
	return LoadTexture(filePath);
}

inline Resource* ResourceSystem::Load(Resource* resource)
{
	if (resource == nullptr)
	{
		return nullptr;
	}
	auto it = m_resourceMap.find(resource->m_filePath);
	if (it != m_resourceMap.end())
	{
		it->second.RefCount++;
		return it->second.Resource.get();
	}
	else
	{
		ResourceContainer container;
		container.RefCount = 1;
		container.Resource.reset(resource);
		m_resourceMap[resource->m_filePath] = std::move(container);
		return resource;
	}
}

#endif
