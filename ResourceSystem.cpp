#include "ResourceSystem.h"
#include "EngineCore.h"
#include "GameContext.h"
#include "RenderingSystem.h"
#include "Texture.h"

ResourceSystem::~ResourceSystem()
{
	for (auto& [key, container] : m_resourceMap)
	{
		if (container.Resource)
		{
			container.Resource->m_resourceSystem = nullptr;
		}
	}
	m_resourceMap.clear();
}

Texture* ResourceSystem::LoadTexture(const std::wstring& key)
{
	// 既に読み込まれている場合は参照カウントを増やして返す
	if (m_resourceMap.count(key))
	{
		m_resourceMap[key].RefCount++;
		return static_cast<Texture*>(m_resourceMap[key].Resource.get());
	}
	auto& device = GetGraphicsDevice();
	auto resource = std::make_unique<Texture>();
	resource->m_filePath = key;
	resource->m_resourceSystem = this;
	resource->Initialize(device);
	Texture* ptr = resource.get();
	ResourceContainer container;
	container.RefCount = 1;
	container.Resource = std::move(resource);
	m_resourceMap[key] = std::move(container);
	return ptr;
}

GraphicsDevice& ResourceSystem::GetGraphicsDevice()
{
	auto& gameContext = m_engineCore->GetGameContext();
	return gameContext.renderingSystem->GetGraphicsDevice();
}

void ResourceSystem::Unload(Resource* resource)
{
	if (resource == nullptr)
	{
		return;
	}
	auto it = m_resourceMap.find(resource->m_filePath);
	if (it != m_resourceMap.end())
	{
		it->second.RefCount--;
		if (it->second.RefCount <= 0)
		{
			if (it->second.Resource)
			{
				it->second.Resource->m_resourceSystem = nullptr;
			}
			m_resourceMap.erase(it);
		}
	}
}
