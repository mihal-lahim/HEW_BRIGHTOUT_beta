#ifndef REOSURCE_SYSTEM_H
#define REOSURCE_SYSTEM_H

#include "Resource.h"
#include "Mesh.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>

class EngineCore;
class GraphicsDevice;
class Texture;

class ResourceSystem
{
public:
	ResourceSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{
	}
	~ResourceSystem();

	// リソース取得(型指定)(Shader、Material、ConstantBuffer、PrimitiveMesh)
	template<typename T>
		requires (std::is_base_of_v<Shader, T> || std::is_base_of_v<Material, T> || std::is_base_of_v<ConstantBufferBase, T> || std::is_base_of_v<PrimitiveMesh, T>)
	T* Load();


	// リソース取得(型指定かつファイルパス指定)(Texture、Model)
	template<typename T>
		requires (std::is_base_of_v<Texture, T>)
	T* Load(const std::wstring& filePath);


	// リソース取得(コピー用)
	Resource* Load(Resource* resource);

	// リソース解放
	void Unload(Resource* resouce);

private:
	// 既存リソースの存在確認
	template<typename T, typename Key>
		requires std::is_base_of_v<Resource, T>
	bool FindExistingResource(const Key& resourceKey) const;

	// ユニークなリソースキーの生成
	size_t MakeUniqueResourceKey(size_t baseKey) const;

	// リソース取得の共通処理
	template<typename T, typename Key, typename... Args>
		requires std::is_base_of_v<Resource, T>
	T* LoadInternal(const Key& resourceKey, Args&&... args);

	// リソースコンテナ構造体
	struct ResourceContainer
	{
		int refCount = 0;
		std::unique_ptr<Resource> instance = nullptr;
	};

	// グラフィックスデバイス取得
	GraphicsDevice& GetGraphicsDevice();

	// 所属するエンジンコア
	EngineCore* m_engineCore = nullptr;

	// リソースマップ
	std::unordered_map<size_t, ResourceContainer> m_resourceMap;
};

#include "ResourceSystem.inl"

#endif
