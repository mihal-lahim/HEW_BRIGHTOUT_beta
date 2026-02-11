#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "DirectXTex.h"
#include "Renderer.h"
#include <DirectXMath.h>
#include "WICTextureLoader11.h"
#include "DirectXTex.h"

#include "ResourceSystem.h"
#include "GameObject.h"

using namespace DirectX;


XMMATRIX AiMatrixToXmMatrix(const aiMatrix4x4& aiMat)
{
	return XMMATRIX(
		aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
	);
}


bool Model::CreateBuffer(GraphicsDevice& device, const std::string& filePath)
{
	m_meshes.clear();
	m_materials.clear();
	m_modelNodes.clear();

	// Assimpのインポーターを作成
	Assimp::Importer importer{};

	// 左手系への変換を適用してモデルを読み込む
	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_ConvertToLeftHanded |
		aiProcess_CalcTangentSpace |
		aiProcess_OptimizeMeshes
	);

	if (!scene || !scene->HasMeshes() || !scene->mRootNode)
	{
		return false;
	}

	// ポインタからインデックスへのマッピング
	PtrToIndexMap ptrToIndexMap{};

	// スケルトンの作成
	CreateSkeleton(scene);

	// メッシュの作成
	m_meshes.reserve(scene->mNumMeshes);
	CreateMesh(device, scene, ptrToIndexMap);

	// マテリアルの作成
	m_materials.reserve(scene->mNumMaterials);
	CreateMaterial(device, scene, ptrToIndexMap);

	// アニメーションの作成
	m_animations.reserve(scene->mNumAnimations);
	CreateAnimation(scene);


	// ルートノードから再帰的にノードを作成
	CreateNode(scene->mRootNode, scene, ptrToIndexMap);

	return true;
}

int Model::CreateNode(const aiNode* node, const aiScene* scene, PtrToIndexMap& map)
{
	// 現在のノードのインデックスを取得
	int thisIndex = static_cast<int>(m_modelNodes.size());

	// 新たなモデルノードを作成
	ModelNode modelNode = {};

	// ノード名の設定
	modelNode.name = node->mName.C_Str();


	for (int i = 0; i < static_cast<int>(node->mNumMeshes); ++i)
	{
		// メッシュインデックスの取得
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		int meshIndex = map.meshToIndexMap[mesh];
		modelNode.meshIndexes.push_back(meshIndex);

		// マテリアルインデックスの取得
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		int materialIndex = map.materialToIndexMap[material];
		modelNode.materialIndexes.push_back(materialIndex);
	}


	// ローカルトランスフォームの設定
	modelNode.transform = XMMatrixTranspose(AiMatrixToXmMatrix(node->mTransformation));

	// モデルノードを追加
	m_modelNodes.push_back(modelNode);

	// 子ノードを再帰的に処理
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		int childIndex = CreateNode(node->mChildren[i], scene, map);
		m_modelNodes[thisIndex].childIndexes.push_back(childIndex);
	}

	return thisIndex;
}

void Model::CreateMesh(GraphicsDevice& device, const aiScene* scene, PtrToIndexMap& ptrToIndex)
{
	for (unsigned int m = 0; m < scene->mNumMeshes; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];

		// メッシュがスキンメッシュかどうかで処理を分岐
		if (mesh->HasBones())
		{
			ConstructSkinnedMesh(device, mesh, ptrToIndex);
		}
		else
		{
			ConstructMesh(device, mesh, ptrToIndex);
		}
	}
}

void Model::ConstructMesh(GraphicsDevice& device, const aiMesh* mesh, PtrToIndexMap& ptrToIndex)
{
	// メッシュの作成
	Mesh newMesh{};

	// 頂点データの準備
	std::vector<Mesh::VertexAttribute> vertices{};

	// 頂点情報の取得
	for (unsigned int v = 0; v < mesh->mNumVertices; v++)
	{
		Mesh::VertexAttribute vertex{};
		// 頂点位置
		vertex.position.x = mesh->mVertices[v].x;
		vertex.position.y = mesh->mVertices[v].y;
		vertex.position.z = mesh->mVertices[v].z;
		// 頂点法線
		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[v].x;
			vertex.normal.y = mesh->mNormals[v].y;
			vertex.normal.z = mesh->mNormals[v].z;
		}
		// テクスチャ座標
		if (mesh->HasTextureCoords(0))
		{
			vertex.uv.x = mesh->mTextureCoords[0][v].x;
			vertex.uv.y = mesh->mTextureCoords[0][v].y;
		}
		// 頂点カラー
		if (mesh->HasVertexColors(0))
		{
			vertex.color.x = mesh->mColors[0][v].r;
			vertex.color.y = mesh->mColors[0][v].g;
			vertex.color.z = mesh->mColors[0][v].b;
			vertex.color.w = mesh->mColors[0][v].a;
		}
		else
		{
			vertex.color.x = 1.0f;
			vertex.color.y = 1.0f;
			vertex.color.z = 1.0f;
			vertex.color.w = 1.0f;
		}
		vertices.push_back(vertex);
	}

	// インデックスデータの準備
	std::vector<unsigned int> indices{};

	// 面情報の取得
	for (unsigned int f = 0; f < mesh->mNumFaces; f++)
	{
		// 面を取得
		const aiFace& face = mesh->mFaces[f];
		for (unsigned int i = 0; i < face.mNumIndices; i++)
		{
			indices.push_back(face.mIndices[i]);
		}
	}

	// メッシュバッファの作成
	newMesh.CreateBuffer(device, vertices, indices);

	// メッシュとインデックスのマッピングを更新
	ptrToIndex.meshToIndexMap[mesh] = static_cast<int>(m_meshes.size());

	// メッシュをモデルに追加
	m_meshes.push_back(std::move(newMesh));
}

void Model::ConstructSkinnedMesh(GraphicsDevice& device, const aiMesh* mesh, PtrToIndexMap& ptrToIndex)
{
	// スキンメッシュの作成
	SkinnedMesh newMesh{};

	// 頂点データの準備
	std::vector<SkinnedMesh::VertexAttribute> vertices{};

	// 頂点情報の取得
	for (unsigned int v = 0; v < mesh->mNumVertices; v++)
	{
		SkinnedMesh::VertexAttribute vertex{};
		// 頂点位置
		vertex.position.x = mesh->mVertices[v].x;
		vertex.position.y = mesh->mVertices[v].y;
		vertex.position.z = mesh->mVertices[v].z;
		// 頂点法線
		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[v].x;
			vertex.normal.y = mesh->mNormals[v].y;
			vertex.normal.z = mesh->mNormals[v].z;
		}
		// テクスチャ座標
		if (mesh->HasTextureCoords(0))
		{
			vertex.uv.x = mesh->mTextureCoords[0][v].x;
			vertex.uv.y = mesh->mTextureCoords[0][v].y;
		}
		// 頂点カラー
		if (mesh->HasVertexColors(0))
		{
			vertex.color.x = mesh->mColors[0][v].r;
			vertex.color.y = mesh->mColors[0][v].g;
			vertex.color.z = mesh->mColors[0][v].b;
			vertex.color.w = mesh->mColors[0][v].a;
		}
		else
		{
			vertex.color.x = 1.0f;
			vertex.color.y = 1.0f;
			vertex.color.z = 1.0f;
			vertex.color.w = 1.0f;
		}


		if (mesh->HasBones())
		{
			// ボーン情報の取得
			for (unsigned int b = 0; b < mesh->mNumBones; b++)
			{
				aiBone* bone = mesh->mBones[b];
				for (unsigned int w = 0; w < bone->mNumWeights; w++)
				{
					// 頂点に影響を与えるボーンか確認
					if (bone->mWeights[w].mVertexId == v)
					{
						for (int i = 0; i < SkinnedMesh::MAX_BONE_INFLUENCE; i++)
						{
							// 空きスロットを探す
							if (vertex.boneIndexes[i] == -1)
							{
								// ボーンインデックスとウェイトを設定
								vertex.boneIndexes[i] = m_skeleton.boneNameToIndexMap[bone->mName.C_Str()];
								vertex.boneWeights[i] = bone->mWeights[w].mWeight;
								break;
							}
						}
					}
				}
			}
		}

		vertices.push_back(vertex);
	}

	// インデックスデータの準備
	std::vector<unsigned int> indices{};

	// 面情報の取得
	for (unsigned int f = 0; f < mesh->mNumFaces; f++)
	{
		// 面を取得
		const aiFace& face = mesh->mFaces[f];
		for (unsigned int i = 0; i < face.mNumIndices; i++)
		{
			indices.push_back(face.mIndices[i]);
		}
	}

	// メッシュバッファの作成
	newMesh.CreateBuffer(device, vertices, indices);

	// メッシュとインデックスのマッピングを更新
	ptrToIndex.skinnedMeshToIndexMap[mesh] = static_cast<int>(m_meshes.size());

	// メッシュをモデルに追加
	m_skinnedMeshes.push_back(std::move(newMesh));
}

void Model::CreateMaterial(GraphicsDevice& device, const aiScene* scene, PtrToIndexMap& ptrToIndex)
{

	for (unsigned int m = 0; m < scene->mNumMaterials; m++)
	{
		// マテリアルを取得
		aiMaterial* material = scene->mMaterials[m];

		// マテリアルの作成
		Material newMaterial{};

		// マテリアルの色情報を取得
		aiColor3D color(0.0f, 0.0f, 0.0f);
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
		{
			newMaterial.SetColor({ color.r, color.g, color.b, 1.0f });
		}


		// テクスチャ読み込み
		for (unsigned int i = 0; i < scene->mNumTextures; i++)
		{
			// 埋め込みテクスチャを取得
			aiTexture* tex = scene->mTextures[i];

			// SRVの作成
			ID3D11ShaderResourceView* texture;

			// 画像データをDirectXTexで読み込む
			DirectX::TexMetadata metadata;
			DirectX::ScratchImage image;
			LoadFromWICMemory((const void*)tex->pcData, tex->mWidth, DirectX::WIC_FLAGS_NONE, &metadata, image);
			CreateShaderResourceView(device.GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
			assert(texture);

			// テクスチャをモデルに登録
			Texture newTexture{};
			newTexture.CreateFromLoaded(device, texture, static_cast<UINT>(metadata.width), static_cast<UINT>(metadata.height));

			// テクスチャをモデルのテクスチャリストに追加
			m_textures.push_back(std::move(newTexture));

			// マテリアルにテクスチャを設定
			newMaterial.texture = &m_textures.back();

		}

		// マテリアルとインデックスのマッピングを更新
		ptrToIndex.materialToIndexMap[material] = static_cast<int>(m_materials.size());

		// マテリアルをモデルに追加
		m_materials.push_back(std::move(newMaterial));
	}
}

void Model::CreateSkeleton(const aiScene* scene)
{
	// 新しいスケルトンを作成
	Skeleton newSkeleton{};

	for (unsigned int m = 0; m < scene->mNumMeshes; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];

		// ボーンが存在する場合
		if(mesh->HasBones())
		{
			for(unsigned int b=0;b<mesh->mNumBones;b++)
			{
				aiBone* bone = mesh->mBones[b];

				// ボーン名の取得
				std::string boneName = bone->mName.C_Str();

				// 既に登録されているか確認
				if(newSkeleton.boneNameToIndexMap.contains(boneName))
				{
					// 既に登録されている場合はスキップ
					continue;
				}

				// 新しいボーンを作成
				Bone newBone{};
				newBone.name = boneName;
				newBone.offsetMatrix = XMMatrixTranspose(AiMatrixToXmMatrix(bone->mOffsetMatrix));

				// ボーンをスケルトンに追加
				int boneIndex = static_cast<int>(newSkeleton.bones.size());
				newSkeleton.bones.push_back(newBone);
				newSkeleton.boneNameToIndexMap[boneName] = boneIndex;
			}
		}
	}
}

void Model::CreateAnimation(const aiScene* scene)
{
	for (unsigned int a = 0; a < scene->mNumAnimations; a++)
	{
		aiAnimation* aiAnim = scene->mAnimations[a];

		// 新しいアニメーションクリップを作成
		AnimationClip newClip{};
		newClip.name = aiAnim->mName.C_Str();
		newClip.duration = static_cast<float>(aiAnim->mDuration);
		newClip.ticksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0f);

		// チャンネルの処理
		for(unsigned int c=0;c<aiAnim->mNumChannels;c++)
		{
			aiNodeAnim* aiChannel = aiAnim->mChannels[c];
			std::string boneName = aiChannel->mNodeName.C_Str();

			// 新しいボーンアニメーションチャンネルを作成
			BoneAnimation boneAnim{};

			// 位置キーフレームの取得
			for (unsigned int k = 0; k < aiChannel->mNumPositionKeys; k++)
			{
				// 位置キーフレームの追加
				aiVectorKey posKey = aiChannel->mPositionKeys[k];
				KeyframeVec3 positionKeyframe{};
				positionKeyframe.time = static_cast<float>(posKey.mTime);
				positionKeyframe.value = { posKey.mValue.x, posKey.mValue.y, posKey.mValue.z };
				boneAnim.positionKeyframes.push_back(positionKeyframe);
			}
			// 回転キーフレームの取得
			for (unsigned int k = 0; k < aiChannel->mNumRotationKeys; k++)
			{
				// 回転キーフレームの追加
				aiQuatKey rotKey = aiChannel->mRotationKeys[k];
				KeyframeQuat rotationKeyframe{};
				rotationKeyframe.time = static_cast<float>(rotKey.mTime);
				rotationKeyframe.value = { rotKey.mValue.x, rotKey.mValue.y, rotKey.mValue.z, rotKey.mValue.w };
				boneAnim.rotationKeyframes.push_back(rotationKeyframe);
			}
			// スケールキーフレームの取得
			for (unsigned int k = 0; k < aiChannel->mNumScalingKeys; k++)
			{
				// スケールキーフレームの追加
				aiVectorKey scaleKey = aiChannel->mScalingKeys[k];
				KeyframeVec3 scaleKeyframe{};
				scaleKeyframe.time = static_cast<float>(scaleKey.mTime);
				scaleKeyframe.value = { scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z };
				boneAnim.scaleKeyframes.push_back(scaleKeyframe);
			}

			// ボーン名からインデックスを取得
			int boneIndex = m_skeleton.boneNameToIndexMap[boneName];

			// ボーンアニメーションをクリップに追加
			newClip.boneAnimations[boneIndex] = boneAnim;
		}
		// クリップをモデルに追加
		m_animations.push_back(std::move(newClip));
	}
}


void ModelPrefab::Instantiate(GameObject& gameObject)
{
	auto* model = gameObject.resource().Load<Model>(m_filePath);

	// ルートノードから再帰的にモデルを構築
	ConstructModel(gameObject, model, 0);
}

void ModelPrefab::ConstructModel(GameObject& parent, const Model* model, int index)
{
	// 現在のノードを取得
	const ModelNode& currentNode = model->GetModelNodes()[index];

	// 新たなゲームオブジェクトを作成
	GameObject* newObject = parent.CreateGameObject();
	newObject->SetName(currentNode.name);

	// トランスフォームの設定
	newObject->transform().SetLocalMatrix(currentNode.transform);

	// 親子関係の設定
	parent.SetChild(*newObject);

	// メッシュとマテリアルの設定
	for (size_t i = 0; i < currentNode.meshIndexes.size(); ++i)
	{
		int meshIndex = currentNode.meshIndexes[i];
		int materialIndex = currentNode.materialIndexes[i];

		// メッシュ用の子オブジェクトを作成
		GameObject* meshObject = parent.CreateGameObject();

		// メッシュレンダラーコンポーネントを追加
		auto* meshRenderer = meshObject->AddComponent<MeshRenderer>();
		meshRenderer->mesh = &model->GetMeshes()[meshIndex];
		meshRenderer->material = model->GetMaterials()[materialIndex];
		meshRenderer->material.vsPath = "MeshVS.cso";
		meshRenderer->material.psPath = "MeshPS.cso";

		// 親子関係の設定
		parent.SetChild(*meshObject);

		// メッシュオブジェクトの名前設定
		std::string meshName = currentNode.name + "_Mesh_" + std::to_string(i);
		meshObject->SetName(meshName);
	}

	// 子ノードを再帰的に処理
	for (int childIndex : currentNode.childIndexes)
	{
		ConstructModel(*newObject, model, childIndex);
	}
}

void SkinnedModelPrefab::Instantiate(GameObject& gameObject)
{
	auto* model = gameObject.resource().Load<Model>(m_filePath);
	gameObject.AddComponent<Animator>(model);

	// ルートノードから再帰的にモデルを構築
	ConstructModel(gameObject, model, 0);
}

void SkinnedModelPrefab::ConstructModel(GameObject& parent, const Model* model, int index)
{
	// 現在のノードを取得
	const ModelNode& currentNode = model->GetModelNodes()[index];

	// 新たなゲームオブジェクトを作成
	GameObject* newObject = parent.CreateGameObject();
	newObject->SetName(currentNode.name);

	// トランスフォームの設定
	newObject->transform().SetLocalMatrix(currentNode.transform);

	// 親子関係の設定
	parent.SetChild(*newObject);

	// メッシュとマテリアルの設定
	for (size_t i = 0; i < currentNode.meshIndexes.size(); ++i)
	{
		int meshIndex = currentNode.meshIndexes[i];
		int materialIndex = currentNode.materialIndexes[i];

		// メッシュ用の子オブジェクトを作成
		GameObject* meshObject = parent.CreateGameObject();

		// メッシュレンダラーコンポーネントを追加
		auto* meshRenderer = meshObject->AddComponent<SkinnedMeshRenderer>();
		meshRenderer->skinnedMesh = &model->GetSkinnedMeshes()[meshIndex];
		meshRenderer->material = model->GetMaterials()[materialIndex];
		meshRenderer->material.vsPath = "SkinnedMeshVS.cso";
		meshRenderer->material.psPath = "MeshPS.cso";

		// 親子関係の設定
		parent.SetChild(*meshObject);

		// メッシュオブジェクトの名前設定
		std::string meshName = currentNode.name + "_Mesh_" + std::to_string(i);
		meshObject->SetName(meshName);
	}

	// 子ノードを再帰的に処理
	for (int childIndex : currentNode.childIndexes)
	{
		ConstructModel(*newObject, model, childIndex);
	}
}
