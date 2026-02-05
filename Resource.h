#ifndef RESOURCE_H
#define RESOURCE_H

#include <cstddef>

class ResourceSystem;
class GraphicsDevice;

class Resource
{
public:
	Resource() = default;
	virtual ~Resource();

	// コピー代入演算子
	Resource* operator=(const Resource* resource);
	// コピーコンストラクタ
	Resource(const Resource* resource);

protected:
	// リソースシステム
	ResourceSystem* m_resourceSystem = nullptr;

	// リソースキー
	size_t m_resourceKey = 0;

	friend class ResourceSystem;
};

#endif
