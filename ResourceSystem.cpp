#include "ResourceSystem.h"
#include "EngineCore.h"
#include "GameContext.h"
#include "RenderingSystem.h"

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
	size_t resourceKey = resource->m_resourceKey;
	if (resourceKey == 0)
	{
		resourceKey = typeid(*resource).hash_code();
		resource->m_resourceKey = resourceKey;
	}
	auto it = m_resourceMap.find(resourceKey);
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
