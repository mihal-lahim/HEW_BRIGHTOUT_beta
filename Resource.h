#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

class ResourceSystem;
class GraphicsDevice;

class Resource
{
public:
	Resource() = default;
	virtual ~Resource();

	// リソース初期化メソッド
	virtual void Initialize(GraphicsDevice& device) = 0;

	// コピー代入演算子
	Resource* operator=(const Resource* resource);
	// コピーコンストラクタ
	Resource(const Resource* resource);

protected:
	// リソースシステム
	ResourceSystem* m_resourceSystem = nullptr;
	// ファイルパス
	std::wstring m_filePath;

	friend class ResourceSystem;
};

#endif
