#ifndef RESOURCE_SYSTEM_INL
#define RESOURCE_SYSTEM_INL

#include "EngineCore.h"
#include "RenderingSystem.h"
#include <Windows.h>
#include "Material.h"
#include <utility>

template<typename T>
	requires (std::is_base_of_v<Shader, T> || std::is_base_of_v<Material, T> || std::is_base_of_v<ConstantBufferBase, T> || std::is_base_of_v<PrimitiveMesh, T>)
inline T* ResourceSystem::Load()
{
	// リソースキーの取得
	size_t resourceKey = typeid(T).hash_code();

	// マテリアルの場合の特別処理
	if constexpr (std::is_base_of_v<Material, T> && requires { typename T::CBType; })
	{
		resourceKey ^= typeid(typename T::CBType).hash_code();
		resourceKey = MakeUniqueResourceKey(resourceKey);
	}
	// プリミティブメッシュの場合の特別処理
	if constexpr (std::is_base_of_v<PrimitiveMesh, T>)
	{
		auto vertexes = T::template CreateVertexes<typename T::VertexAttribute>();
		auto indexes = T::CreateIndexes();
		return LoadInternal<T>(resourceKey, vertexes, indexes);
	}
	else
	{
		// リソースの生成
		return LoadInternal<T>(resourceKey);
	}
}

template<typename T>
	requires (std::is_base_of_v<Texture, T>)
inline T* ResourceSystem::Load(const std::wstring& filePath)
{
	// リソースキーの取得
	size_t resourceKey = std::hash<std::wstring>{}(filePath);

	// リソースの生成
	return LoadInternal<T>(resourceKey, filePath);
}

template<typename T, typename Key, typename... Args>
	requires std::is_base_of_v<Resource, T>
inline T* ResourceSystem::LoadInternal(const Key& resourceKey, Args&&... args)
{
	// 既存リソースの確認
	if (FindExistingResource<T>(resourceKey))
	{
		auto it = m_resourceMap.find(resourceKey);
		it->second.refCount++;
		return static_cast<T*>(it->second.instance.get());
	}

	// デバイスの取得
	auto& device = GetGraphicsDevice();

	// リソースの生成と初期化
	auto resource = std::make_unique<T>();
	resource->m_resourceKey = static_cast<size_t>(resourceKey);
	resource->m_resourceSystem = this;
	bool result = false;
	if constexpr (requires { resource->CreateBuffer(device, std::forward<Args>(args)...); })
	{
		result = resource->CreateBuffer(device, std::forward<Args>(args)...);
	}
	else if constexpr (requires { resource->template CreateBuffer<typename T::VertexAttribute>(device, std::forward<Args>(args)...); })
	{
		result = resource->template CreateBuffer<typename T::VertexAttribute>(device, std::forward<Args>(args)...);
	}
	if (!result)
	{
		MessageBox(NULL, "リソースの初期化に失敗しました。", "Error", MB_OK | MB_ICONERROR);
	}

	T* ptr = resource.get();
	ResourceContainer container;
	container.refCount = 1;
	container.instance = std::move(resource);
	m_resourceMap[static_cast<size_t>(resourceKey)] = std::move(container);
	return ptr;
}

template<typename T, typename Key>
	requires std::is_base_of_v<Resource, T>
inline bool ResourceSystem::FindExistingResource(const Key& resourceKey) const
{
	auto it = m_resourceMap.find(static_cast<size_t>(resourceKey));
	if (it != m_resourceMap.end())
	{
		return true;
	}
	return false;
}

inline Resource* ResourceSystem::Load(Resource* resource)
{
	if (resource == nullptr)
	{
		return nullptr;
	}

	// リソースキーの取得
	size_t resourceKey = resource->m_resourceKey;
	if (resourceKey == 0)
	{
		resourceKey = typeid(*resource).hash_code();
		resource->m_resourceKey = resourceKey;
	}

	if (FindExistingResource<Resource>(resourceKey))
	{
		auto it = m_resourceMap.find(resourceKey);
		it->second.refCount++;
		return it->second.instance.get();
	}
	ResourceContainer container;
	container.refCount = 1;
	container.instance.reset(resource);
	m_resourceMap[resourceKey] = std::move(container);
	return resource;
}

#endif
