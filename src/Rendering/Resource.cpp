#include "Resource.h"
#include "ResourceSystem.h"

Resource::~Resource()
{
	if (m_resourceSystem != nullptr)
	{
		m_resourceSystem->Unload(this);
	}
}

Resource* Resource::operator=(const Resource* resource)
{
	m_resourceSystem = resource->m_resourceSystem;
	m_resourceKey = resource->m_resourceKey;
	if (m_resourceSystem)
	{
		m_resourceSystem->Copy(this);
	}
	return this;
}

Resource::Resource(const Resource* resource)
{
	m_resourceSystem = resource->m_resourceSystem;
	m_resourceKey = resource->m_resourceKey;
	if (m_resourceSystem)
	{
		m_resourceSystem->Copy(this);
	}
}
