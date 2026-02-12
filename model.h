#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Resource.h"
#include "Prefab.h"
#include "Animation.h"
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma comment (lib, "assimp-vc143-mt.lib")

// モデルノード構造体
struct ModelNode
{
	std::string name = {};
	std::vector<int> meshIndexes = {};
	std::vector<int> skinnedMeshIndexes = {};
	std::vector<int> materialIndexes = {};
	std::vector<int> childIndexes = {};
	std::vector<int> boneIndexes = {};
	DirectX::XMMATRIX transform = {};
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

	// マテリアル群の取得
	const std::vector<Material>& GetMaterials() const
	{
		return m_materials;
	}

	// スケルトンの取得
	const Skeleton& GetSkeleton() const
	{
		return m_skeleton;
	}

	// アニメーション群の取得
	const std::vector<AnimationClip>& GetAnimations() const
	{
		return m_animations;
	}

	// モデルノード群の取得
	const std::vector<ModelNode>& GetModelNodes() const
	{
		return m_modelNodes;
	}
private:
	// メッシュ群
	std::vector<Mesh> m_meshes = {};

	// スキンメッシュ群
	std::vector<SkinnedMesh> m_skinnedMeshes = {};

	// マテリアル群
	std::vector<Material> m_materials = {};

	// テクスチャ群
	std::vector<Texture> m_textures = {};

	// スケルトン（ボーン群）
	Skeleton m_skeleton = {};

	// アニメーション群
	std::vector<AnimationClip> m_animations = {};

	// モデルノード群
	std::vector<ModelNode> m_modelNodes = {};

	// ポインタからインデックスへのマッピング構造体
	struct PtrToIndexMap
	{
		std::unordered_map<const aiMesh*, int> meshToIndexMap = {};
		std::unordered_map<const aiMesh*, int> skinnedMeshToIndexMap = {};
		std::unordered_map<const aiMaterial*, int> materialToIndexMap = {};
	};

	// モデルノード処理構築メソッド
	int CreateNode(const aiNode* node, const aiScene* scene, PtrToIndexMap& map);

	// メッシュ構築メソッド
	void CreateMesh(GraphicsDevice& device, const aiScene* scene, PtrToIndexMap& ptrToIndex);

	// メッシュ構築ヘルパーメソッド
	void ConstructMesh(GraphicsDevice& device, const aiMesh* mesh, PtrToIndexMap& ptrToIndex);
	void ConstructSkinnedMesh(GraphicsDevice& device, const aiMesh* mesh, PtrToIndexMap& ptrToIndex);


	// マテリアル構築メソッド
	void CreateMaterial(GraphicsDevice& device, const aiScene* scene, PtrToIndexMap& ptrToIndex);

	// スケルトン構築メソッド
	void CreateSkeleton(const aiScene* scene);

	// アニメーション構築メソッド
	void CreateAnimation(const aiScene* scene);
};


class ModelPrefab : public Prefab
{
public:
	ModelPrefab(const std::string& filePath)
		: m_filePath(filePath)
	{
	}

	void Instantiate(GameObject& gameObject) override;

private:
	// モデルファイルパス
	std::string m_filePath = {};

	// モデル構築メソッド
	void ConstructModel(GameObject& parent, const Model* model, int index);
};


class SkinnedModelPrefab : public Prefab
{
public:
	SkinnedModelPrefab(const std::string& filePath)
		: m_filePath(filePath)
	{
	}
	void Instantiate(GameObject& gameObject) override;

private:
	// モデルファイルパス
	std::string m_filePath = {};

	// モデル構築メソッド
	void ConstructModel(GameObject& parent, const Model* model, int index);
};

#endif
