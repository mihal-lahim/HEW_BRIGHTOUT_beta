#ifndef RESOURCE_SYSTEM_INL
#define RESOURCE_SYSTEM_INL

#include "ResourceSystem.h"
#include "Mesh.h"
#include "Texture.h"

template<typename T>
	requires std::is_base_of_v<Resource, T>
inline T* ResourceSystem::Load()
{
	size_t resourceKey = typeid(T).hash_code();
	auto it = m_resourceMap.find(resourceKey);
	if (it != m_resourceMap.end())
	{
		it->second.RefCount++;
		return static_cast<T*>(it->second.Resource.get());
	}

	auto& device = GetGraphicsDevice();

	auto resource = std::make_unique<T>();
	resource->m_resourceKey = resourceKey;
	resource->m_resourceSystem = this;
	resource->CreateBuffers(device);
	T* ptr = resource.get();
	ResourceContainer container;
	container.RefCount = 1;
	container.Resource = std::move(resource);
	m_resourceMap[resourceKey] = std::move(container);
	return ptr;
}

template<typename T>
	requires std::is_base_of_v<Resource, T>
inline T* ResourceSystem::Load(const std::wstring& filePath)
{
	size_t resourceKey = std::hash<std::wstring>{}(filePath);
	auto it = m_resourceMap.find(resourceKey);
	if (it != m_resourceMap.end())
	{
		it->second.RefCount++;
		return static_cast<T*>(it->second.Resource.get());
	}

	auto& device = GetGraphicsDevice();

	auto resource = std::make_unique<T>();
	if constexpr (std::is_same_v<T, Texture>)
	{
		resource->SetFilePath(filePath);
	}
	resource->m_resourceKey = resourceKey;
	resource->m_resourceSystem = this;
	resource->CreateBuffers(device);
	T* ptr = resource.get();
	ResourceContainer container;
	container.RefCount = 1;
	container.Resource = std::move(resource);
	m_resourceMap[resourceKey] = std::move(container);
	return ptr;
}

inline Resource* ResourceSystem::Load(Resource* resource)
{
	if (resource == nullptr)
	{
		return nullptr;
	}
	size_t resourceKey = resource->m_resourceKey;
	if (resourceKey == 0)
	{
		resourceKey = typeid(*resource).hash_code();
		resource->m_resourceKey = resourceKey;
	}
	auto it = m_resourceMap.find(resourceKey);
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
		m_resourceMap[resourceKey] = std::move(container);
		return resource;
	}
}

#endif
