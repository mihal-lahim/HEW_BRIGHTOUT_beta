#include "ResourceSystem.h"
#include "EngineCore.h"
#include "GameContext.h"
#include "RenderingSystem.h"

ResourceSystem::~ResourceSystem()
{
	for (auto& [key, container] : m_resourceMap)
	{
		if (container.instance)
		{
			container.instance->m_resourceSystem = nullptr;
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
		it->second.refCount--;
		if (it->second.refCount <= 0)
		{
			if (it->second.instance)
			{
				it->second.instance->m_resourceSystem = nullptr;
			}
			m_resourceMap.erase(it);
		}
	}
}

size_t ResourceSystem::MakeUniqueResourceKey(size_t baseKey) const
{
	size_t counter = 1;
	size_t uniqueKey = baseKey;

	// クヌース・ラトマーの定数
	static constexpr size_t kruthmuller_constant = 0x9e3779b97f4a7c15ULL;

	// ユニークなキーが見つかるまでループ
	while (m_resourceMap.contains(baseKey))
	{
		uniqueKey = baseKey ^ (counter * kruthmuller_constant);
	}

	return uniqueKey;
}
