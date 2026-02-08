#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Texture.h"
#include "Animation.h"
#include "Resource.h"
#include "ResourceSystem.h"
#include "Prefab.h"
#include <string>
#include <filesystem>
#include "GameObject.h"

#include "assimp/scene.h"
#pragma comment (lib, "assimp-vc143-mt.lib")


struct ModelNode
{
	std::string name = {};
	int meshIndex = -1;
	int skinnedMeshIndex = -1;
	int textureIndex = -1;
	int parent = -1;
	std::vector<int> children = {};
	DirectX::XMMATRIX localMatrix = {};
};

class Model : public Resource
{
public:
	// モデルの読み込み
	bool CreateBuffer(GraphicsDevice& device, const std::string& filePath);

	// メッシュ群の取得
	const std::vector<Mesh>& GetMeshes() const
	{
		return m_meshes;
	}
	// スキンメッシュ群の取得
	const std::vector<SkinnedMesh>& GetSkinnedMeshes() const
	{
		return m_skinnedMeshes;
	}
	// テクスチャ群の取得
	const std::vector<Texture>& GetTextures() const
	{
		return m_textures;
	}
	// スケルトンの取得
	const Skeleton& GetSkeleton() const
	{
		return m_skeleton;
	}
	// アニメーションクリップ群の取得
	const std::vector<AnimationClip>& GetAnimationClips() const
	{
		return m_animationClips;
	}

	// ノード群の取得
	const std::vector<ModelNode>& GetModelNodes() const
	{
		return m_nodes;
	}

private:
	// ノード群
	std::vector<ModelNode> m_nodes = {};

	// メッシュ群
	std::vector<Mesh> m_meshes = {};
	// スキンメッシュ群
	std::vector<SkinnedMesh> m_skinnedMeshes = {};

	// テクスチャ群
	std::vector<Texture> m_textures = {};

	// スケルトン
	Skeleton m_skeleton = {};
	std::vector<AnimationClip> m_animationClips = {};

	std::filesystem::path m_baseDirectory = {};

	// ノードの処理
	int ProcessNode(GraphicsDevice& device, aiNode* node, const aiScene* scene, int parentIndex);

	// メッシュの読み込み
	int LoadMesh(GraphicsDevice& device, aiMesh* aimesh, const aiScene* aiscene);
	// スキンメッシュの読み込み
	int LoadSkinnedMesh(GraphicsDevice& device, aiMesh* aimesh, const aiScene* aiscene);

	// テクスチャの読み込み
	void LoadTextures(GraphicsDevice& device, const aiScene* scene);

	// スケルトンの読み込み
	void LoadSkeleton(const aiScene* scene);

	// アニメーションクリップの読み込み
	void LoadAnimationClips(const aiScene* scene);
};



// モデルプレハブクラス
class ModelPrefab : public Prefab
{
public:
	ModelPrefab(const std::string& filePath)
		: m_filePath(filePath)
	{
	}
	// インスタンス化メソッド
	void Instantiate(GameObject& gameObject) override;

private:
	std::string m_filePath = {};
};

// アニメーションモデルプレハブクラス
class SkinnedModelPrefab : public Prefab
{
public:
	SkinnedModelPrefab(const std::string& filePath)
		: m_filePath(filePath)
	{
	}
	// インスタンス化メソッド
	void Instantiate(GameObject& gameObject) override;

private:
	std::string m_filePath = {};
};


class CubePrefab : public Prefab
{
public:
	void Instantiate(GameObject& gameObject) override;
};

class SpherePrefab : public Prefab
{
public:
	void Instantiate(GameObject& gameObject) override;
};

#endif
