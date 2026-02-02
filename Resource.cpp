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
	m_filePath = resource->m_filePath;
	m_resourceSystem->Load(this);
	return this;
}

Resource::Resource(const Resource* resource)
{
	m_resourceSystem = resource->m_resourceSystem;
	m_filePath = resource->m_filePath;
	m_resourceSystem->Load(this);
}
