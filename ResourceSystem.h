#ifndef REOSURCE_SYSTEM_H
#define REOSURCE_SYSTEM_H

#include "Resource.h"
#include <memory>
#include <unordered_map>

class CubeMesh;
class PlaneMesh;
class SphereMesh;
class CapsuleMesh;
class CylinderMesh;
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

	// リソース取得
	template<typename T>
		requires std::is_base_of_v<Resource, T>
	T* Load();

	// リソース取得
	Resource* Load(Resource* resource);

	// リソース取得
	template<typename T>
		requires std::is_base_of_v<Resource, T>
	T* Load(const std::wstring& filePath);

	// リソース解放
	void Unload(Resource* resouce);

private:
	// リソースコンテナ構造体
	struct ResourceContainer
	{
		int RefCount = 0;
		std::unique_ptr<Resource> Resource = nullptr;
	};

	template<typename T>
		requires std::is_base_of_v<Resource, T>
	T* LoadPrimitiveMesh(const std::wstring& key);

	// テクスチャ読み込み
	Texture* LoadTexture(const std::wstring& key);

	GraphicsDevice& GetGraphicsDevice();

	// 所属するエンジンコア
	EngineCore* m_engineCore = nullptr;

	// リソースマップ
	std::unordered_map<std::wstring, ResourceContainer> m_resourceMap;
};

#include "ResourceSystem.inl"

#endif
