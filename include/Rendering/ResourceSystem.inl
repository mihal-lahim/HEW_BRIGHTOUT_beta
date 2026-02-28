#ifndef RESOURCE_SYSTEM_INL
#define RESOURCE_SYSTEM_INL

#include "EngineCore.h"
#include "RenderingSystem.h"
#include <Windows.h>
#include "Material.h"
#include <tuple>
#include <utility>

template<typename T, typename... Args>
	requires std::is_base_of_v<Resource, T>
inline T* ResourceSystem::Load(Args&&... args)
{
	// テクスチャの場合の特別処理
	if constexpr (std::is_base_of_v<Texture, T>)
	{
		auto filePath = static_cast<std::wstring>(std::get<0>(std::forward_as_tuple(args...)));
		size_t resourceKey = std::hash<std::wstring>{}(filePath);
		return LoadInternal<T>(resourceKey, filePath);
	}
	// シェーダーとモデル場合の特別処理
	else if constexpr (std::is_base_of_v <Shader, T> || std::is_base_of_v <Model, T>)
	{
		auto filePath = static_cast<std::string>(std::get<0>(std::forward_as_tuple(args...)));
		size_t resourceKey = std::hash<std::string>{}(filePath);
		return LoadInternal<T>(resourceKey, filePath);
	}
	// シェーダープログラムの場合の特別処理
	else if constexpr (std::is_base_of_v<ShaderProgram, T>)
	{
		auto vsFilePath = static_cast<std::string>(std::get<0>(std::forward_as_tuple(args...)));
		auto psFilePath = static_cast<std::string>(std::get<1>(std::forward_as_tuple(args...)));
		size_t resourceKey = std::hash<std::string>{}(vsFilePath) ^ std::hash<std::string>{}(psFilePath);
		return LoadInternal<T>(resourceKey, vsFilePath, psFilePath);
	}
	else
	{
		// リソースキーの取得
		size_t resourceKey = typeid(T).hash_code();

		// マテリアルの場合の特別処理
		if constexpr (std::is_base_of_v<Material, T>)
		{
			resourceKey = MakeUniqueResourceKey(resourceKey);
			return LoadInternal<T>(resourceKey, std::forward<Args>(args)...);
		}
	}
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

inline Resource* ResourceSystem::Copy(Resource* resource)
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
