#include "Model.h"

#include "DirectXTex.h"
#include "Renderer.h"
#include <DirectXMath.h>
#include <filesystem>
#include <functional>
#include <unordered_set>
#include <wrl/client.h>

#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "assimp/version.h"

using namespace DirectX;

namespace
{
	DirectX::XMMATRIX ToMatrix(const aiMatrix4x4& matrix)
	{
		return DirectX::XMMATRIX(
			matrix.a1, matrix.b1, matrix.c1, matrix.d1,
			matrix.a2, matrix.b2, matrix.c2, matrix.d2,
			matrix.a3, matrix.b3, matrix.c3, matrix.d3,
			matrix.a4, matrix.b4, matrix.c4, matrix.d4
		);
	}

	DirectX::XMMATRIX ConvertMatrix(const aiMatrix4x4& matrix)
	{
		const DirectX::XMMATRIX convert = DirectX::XMMatrixScaling(1.0f, 1.0f, -1.0f);
		return convert * ToMatrix(matrix) * convert;
	}

	DirectX::XMFLOAT3 ConvertVector(const aiVector3D& value)
	{
		return { value.x, value.y, -value.z };
	}

	DirectX::XMFLOAT4 ConvertQuaternion(const aiQuaternion& value)
	{
		const DirectX::XMMATRIX convert = DirectX::XMMatrixScaling(1.0f, 1.0f, -1.0f);
		const DirectX::XMVECTOR quat = DirectX::XMVectorSet(value.x, value.y, value.z, value.w);
		const DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(quat);
		const DirectX::XMMATRIX converted = convert * rotation * convert;
		const DirectX::XMVECTOR convertedQuat = DirectX::XMQuaternionRotationMatrix(converted);
		DirectX::XMFLOAT4 result = {};
		DirectX::XMStoreFloat4(&result, convertedQuat);
		return result;
	}

	std::string MakeNodeName(const ModelNode& node, size_t)
	{
		return node.name;
	}

	std::vector<GameObject*> CreateNodeObjects(GameObject& root, const std::vector<ModelNode>& nodes)
	{
		std::vector<GameObject*> objects(nodes.size(), nullptr);

		for (size_t i = 0; i < nodes.size(); ++i)
		{
			const auto& node = nodes[i];
			GameObject* child = root.CreateGameObject();
			child->SetName(MakeNodeName(node, i));
			child->transform().SetLocalMatrix(node.localMatrix);
			objects[i] = child;
		}

		for (size_t i = 0; i < nodes.size(); ++i)
		{
			const auto& node = nodes[i];
			if (node.parent >= 0 && node.parent < static_cast<int>(nodes.size()))
			{
				objects[i]->SetParent(*objects[node.parent]);
			}
			else
			{
				objects[i]->SetParent(root);
			}
		}

		return objects;
	}
}

#include "DebugOstream.h"

bool Model::CreateBuffer(GraphicsDevice& device, const std::string& filePath)
{


	Assimp::Importer importer;

	m_meshes.clear();
	m_skinnedMeshes.clear();
	m_textures.clear();
	m_nodes.clear();
	m_skeleton = {};
	m_animationClips.clear();

	m_baseDirectory = std::filesystem::path(filePath).parent_path();

	const aiScene* scene = importer.ReadFile(
		filePath,
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality
		// | aiProcess_PreTransformVertices
	);

	hal::dout << importer.GetErrorString() << std::endl;

	if (!scene || !scene->mRootNode)
	{
		return false;
	}

	LoadSkeleton(scene);
	ProcessNode(device, scene->mRootNode, scene, -1);
	LoadTextures(device, scene);
	LoadAnimationClips(scene);

	return true;
}

int Model::ProcessNode(GraphicsDevice& device, aiNode* node, const aiScene* scene, int parentIndex)
{
	ModelNode modelNode = {};
	modelNode.name = node->mName.C_Str();
	modelNode.parent = parentIndex;
	modelNode.localMatrix = ConvertMatrix(node->mTransformation);

	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		if (aimesh->HasBones())
		{
			const int skinnedMeshIndex = LoadSkinnedMesh(device, aimesh, scene);
			if (modelNode.skinnedMeshIndex < 0 && skinnedMeshIndex >= 0)
			{
				modelNode.skinnedMeshIndex = skinnedMeshIndex;
			}
		}
		else
		{
			const int meshIndex = LoadMesh(device, aimesh, scene);
			if (modelNode.meshIndex < 0 && meshIndex >= 0)
			{
				modelNode.meshIndex = meshIndex;
			}
		}
	}

	const int nodeIndex = static_cast<int>(m_nodes.size());
	m_nodes.push_back(std::move(modelNode));

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		const int childIndex = ProcessNode(device, node->mChildren[i], scene, nodeIndex);
		m_nodes[nodeIndex].children.push_back(childIndex);
	}

	return nodeIndex;
}

int Model::LoadMesh(GraphicsDevice& device, aiMesh* aimesh, const aiScene*)
{

	std::vector<Mesh::VertexAttribute> vertexes = {};
	vertexes.resize(aimesh->mNumVertices);

	for (unsigned int i = 0; i < aimesh->mNumVertices; ++i)
	{
		auto& vertex = vertexes[i];
		const aiVector3D& position = aimesh->mVertices[i];
		vertex.position = ConvertVector(position);
		vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (aimesh->HasNormals())
		{
			const aiVector3D& normal = aimesh->mNormals[i];
			vertex.normal = ConvertVector(normal);
		}
		if (aimesh->HasTextureCoords(0))
		{
			const aiVector3D& uv = aimesh->mTextureCoords[0][i];
			vertex.uv = { uv.x, uv.y };
		}
	}

	std::vector<UINT> indexes = {};
	indexes.reserve(aimesh->mNumFaces * 3);

	for (unsigned int i = 0; i < aimesh->mNumFaces; ++i)
	{
		const aiFace& face = aimesh->mFaces[i];
		if (face.mNumIndices >= 3)
		{
			indexes.push_back(static_cast<UINT>(face.mIndices[0]));
			indexes.push_back(static_cast<UINT>(face.mIndices[2]));
			indexes.push_back(static_cast<UINT>(face.mIndices[1]));
			for (unsigned int j = 3; j < face.mNumIndices; ++j)
			{
				indexes.push_back(static_cast<UINT>(face.mIndices[j]));
			}
		}
	}

	Mesh mesh = {};
	if (mesh.CreateBuffer(device, vertexes, indexes))
	{
		const int meshIndex = static_cast<int>(m_meshes.size());
		m_meshes.push_back(std::move(mesh));
		return meshIndex;
	}

	return -1;
}

int Model::LoadSkinnedMesh(GraphicsDevice& device, aiMesh* aimesh, const aiScene* aiscene)
{
	(void)aiscene;

	std::vector<SkinnedMesh::VertexAttribute> vertexes = {};
	vertexes.resize(aimesh->mNumVertices);

	for (unsigned int i = 0; i < aimesh->mNumVertices; ++i)
	{
		auto& vertex = vertexes[i];
		const aiVector3D& position = aimesh->mVertices[i];
		vertex.position = ConvertVector(position);
		vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (aimesh->HasNormals())
		{
			const aiVector3D& normal = aimesh->mNormals[i];
			vertex.normal = ConvertVector(normal);
		}
		if (aimesh->HasTextureCoords(0))
		{
			const aiVector3D& uv = aimesh->mTextureCoords[0][i];
			vertex.uv = { uv.x, uv.y };
		}
	}

	for (unsigned int i = 0; i < aimesh->mNumBones; ++i)
	{
		const aiBone* bone = aimesh->mBones[i];
		const std::string boneName = bone->mName.C_Str();
		int boneIndex = m_skeleton.FindBoneIndex(boneName);
		if (boneIndex < 0)
		{
			Bone newBone = {};
			newBone.name = boneName;
			newBone.parentIndex = -1;
			newBone.offsetMatrix = ConvertMatrix(bone->mOffsetMatrix);
			newBone.bindPose = DirectX::XMMatrixIdentity();
			m_skeleton.boneMap[newBone.name] = static_cast<int>(m_skeleton.bones.size());
			m_skeleton.bones.push_back(std::move(newBone));
			boneIndex = static_cast<int>(m_skeleton.bones.size()) - 1;
		}

		for (unsigned int j = 0; j < bone->mNumWeights; ++j)
		{
			const aiVertexWeight& weight = bone->mWeights[j];
			auto& vertex = vertexes[weight.mVertexId];
			for (int k = 0; k < 4; ++k)
			{
				if (vertex.weight[k] == 0.0f)
				{
					vertex.bone[k] = static_cast<UINT>(boneIndex);
					vertex.weight[k] = weight.mWeight;
					break;
				}
			}
		}
	}

	std::vector<UINT> indexes = {};
	indexes.reserve(aimesh->mNumFaces * 3);

	for (unsigned int i = 0; i < aimesh->mNumFaces; ++i)
	{
		const aiFace& face = aimesh->mFaces[i];
		if (face.mNumIndices >= 3)
		{
			indexes.push_back(static_cast<UINT>(face.mIndices[0]));
			indexes.push_back(static_cast<UINT>(face.mIndices[2]));
			indexes.push_back(static_cast<UINT>(face.mIndices[1]));
			for (unsigned int j = 3; j < face.mNumIndices; ++j)
			{
				indexes.push_back(static_cast<UINT>(face.mIndices[j]));
			}
		}
	}

	SkinnedMesh mesh = {};
	if (mesh.CreateBuffer(device, vertexes, indexes))
	{
		const int meshIndex = static_cast<int>(m_skinnedMeshes.size());
		m_skinnedMeshes.push_back(std::move(mesh));
		return meshIndex;
	}

	return -1;
}

void Model::LoadTextures(GraphicsDevice& device, const aiScene* scene)
{
	std::unordered_set<std::string> loadedTextures = {};

	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];
		aiString path = {};

		const aiTextureType textureTypes[] = { aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE };
		for (auto textureType : textureTypes)
		{
			const unsigned int textureCount = material->GetTextureCount(textureType);
			for (unsigned int j = 0; j < textureCount; ++j)
			{
				if (material->GetTexture(textureType, j, &path) != AI_SUCCESS)
				{
					continue;
				}

				const std::string texturePath = path.C_Str();
				if (!loadedTextures.insert(texturePath).second)
				{
					continue;
				}

				if (!texturePath.empty() && texturePath[0] == '*')
				{
					const aiTexture* embedded = scene->GetEmbeddedTexture(texturePath.c_str());
					if (!embedded || embedded->mHeight != 0)
					{
						continue;
					}

					DirectX::ScratchImage image = {};
					DirectX::TexMetadata metadata = {};
					if (FAILED(DirectX::LoadFromWICMemory(
						reinterpret_cast<const uint8_t*>(embedded->pcData),
						static_cast<size_t>(embedded->mWidth),
						DirectX::WIC_FLAGS_NONE,
						&metadata,
						image)))
					{
						continue;
					}

					Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;
					if (FAILED(DirectX::CreateShaderResourceView(
						device.GetDevice(),
						image.GetImages(),
						image.GetImageCount(),
						metadata,
						&srv)))
					{
						continue;
					}

					Texture texture = {};
					texture.CreateFromLoaded(device, srv.Detach(), static_cast<UINT>(metadata.width), static_cast<UINT>(metadata.height));
					m_textures.push_back(std::move(texture));
				}
				else
				{
					std::filesystem::path resolvedPath = std::filesystem::path(texturePath);
					if (resolvedPath.is_relative())
					{
						resolvedPath = m_baseDirectory / resolvedPath;
					}

					Texture texture = {};
					if (texture.CreateBuffer(device, resolvedPath.wstring()))
					{
						m_textures.push_back(std::move(texture));
					}
				}
			}
		}
	}
}

void Model::LoadSkeleton(const aiScene* scene)
{
	std::unordered_set<std::string> boneNames = {};

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* aimesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < aimesh->mNumBones; ++j)
		{
			boneNames.insert(aimesh->mBones[j]->mName.C_Str());
		}
	}

	for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* animation = scene->mAnimations[i];
		for (unsigned int j = 0; j < animation->mNumChannels; ++j)
		{
			boneNames.insert(animation->mChannels[j]->mNodeName.C_Str());
		}
	}

	std::function<void(aiNode*, int)> buildSkeleton = [&](aiNode* node, int parentIndex)
	{
		const std::string nodeName = node->mName.C_Str();
		int currentIndex = parentIndex;
		if (boneNames.find(nodeName) != boneNames.end())
		{
			Bone bone = {};
			bone.name = nodeName;
			bone.parentIndex = parentIndex;
			bone.offsetMatrix = DirectX::XMMatrixIdentity();
			bone.bindPose = ConvertMatrix(node->mTransformation);
			m_skeleton.boneMap[bone.name] = static_cast<int>(m_skeleton.bones.size());
			m_skeleton.bones.push_back(std::move(bone));
			currentIndex = static_cast<int>(m_skeleton.bones.size()) - 1;
		}

		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			buildSkeleton(node->mChildren[i], currentIndex);
		}
	};

	buildSkeleton(scene->mRootNode, -1);

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* aimesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < aimesh->mNumBones; ++j)
		{
			aiBone* bone = aimesh->mBones[j];
			int index = m_skeleton.FindBoneIndex(bone->mName.C_Str());
			if (index >= 0)
			{
				m_skeleton.bones[index].offsetMatrix = ConvertMatrix(bone->mOffsetMatrix);
			}
		}
	}
}

void Model::LoadAnimationClips(const aiScene* scene)
{
	for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* animation = scene->mAnimations[i];
		AnimationClip clip = {};
		clip.name = animation->mName.C_Str();
		clip.duration = static_cast<float>(animation->mDuration);
		clip.ticksPerSecond = animation->mTicksPerSecond == 0.0 ? 25.0f : static_cast<float>(animation->mTicksPerSecond);
		clip.boneTracks.resize(m_skeleton.bones.size());

		for (unsigned int j = 0; j < animation->mNumChannels; ++j)
		{
			aiNodeAnim* channel = animation->mChannels[j];
			const int boneIndex = m_skeleton.FindBoneIndex(channel->mNodeName.C_Str());
			if (boneIndex < 0)
			{
				continue;
			}

			BoneKeyframes& keyframes = clip.boneTracks[boneIndex];

			for (unsigned int k = 0; k < channel->mNumPositionKeys; ++k)
			{
				const aiVectorKey& key = channel->mPositionKeys[k];
				keyframes.positionKeyframes.push_back({
					static_cast<float>(key.mTime),
					ConvertVector(key.mValue)
				});
			}

			for (unsigned int k = 0; k < channel->mNumRotationKeys; ++k)
			{
				const aiQuatKey& key = channel->mRotationKeys[k];
				keyframes.rotationKeyframes.push_back({
					static_cast<float>(key.mTime),
					ConvertQuaternion(key.mValue)
				});
			}

			for (unsigned int k = 0; k < channel->mNumScalingKeys; ++k)
			{
				const aiVectorKey& key = channel->mScalingKeys[k];
				keyframes.scaleKeyframes.push_back({
					static_cast<float>(key.mTime),
					ConvertVector(key.mValue)
				});
			}
		}

		m_animationClips.push_back(std::move(clip));
	}
}

void ModelPrefab::Instantiate(GameObject& gameObject)
{
	Model* model = gameObject.resource().Load<Model>(m_filePath);
	if (!model)
	{
		return;
	}

	const auto& nodes = model->GetModelNodes();
	const auto& meshes = model->GetMeshes();
	std::vector<GameObject*> nodeObjects = CreateNodeObjects(gameObject, nodes);

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		const auto& node = nodes[i];
		if (node.meshIndex >= 0 && node.meshIndex < static_cast<int>(meshes.size()))
		{
			auto* renderer = nodeObjects[i]->AddComponent<MeshRenderer>();
			renderer->mesh = &meshes[node.meshIndex];
		}
	}
}

void SkinnedModelPrefab::Instantiate(GameObject& gameObject)
{
	Model* model = gameObject.resource().Load<Model>(m_filePath);
	if (!model)
	{
		return;
	}

	const auto& nodes = model->GetModelNodes();
	const auto& meshes = model->GetMeshes();
	const auto& skinnedMeshes = model->GetSkinnedMeshes();
	std::vector<GameObject*> nodeObjects = CreateNodeObjects(gameObject, nodes);

	const auto& skeleton = model->GetSkeleton();
	std::vector<GameObject*> boneObjects(skeleton.bones.size(), nullptr);
	std::vector<Transform*> boneTransforms = {};
	boneTransforms.reserve(skeleton.bones.size());

	for (size_t i = 0; i < skeleton.bones.size(); ++i)
	{
		const Bone& bone = skeleton.bones[i];
		GameObject* boneObject = gameObject.CreateGameObject();
		boneObject->SetName(bone.name);
		boneObject->transform().SetLocalMatrix(bone.bindPose);
		boneObjects[i] = boneObject;
		boneTransforms.push_back(&boneObject->transform());
	}

	for (size_t i = 0; i < skeleton.bones.size(); ++i)
	{
		const Bone& bone = skeleton.bones[i];
		if (bone.parentIndex >= 0 && bone.parentIndex < static_cast<int>(skeleton.bones.size()))
		{
			boneObjects[i]->SetParent(*boneObjects[bone.parentIndex]);
		}
		else
		{
			boneObjects[i]->SetParent(gameObject);
		}
	}

	auto* animationController = gameObject.AddComponent<AnimationController>();
	animationController->Setup(model, boneTransforms);

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		const auto& node = nodes[i];
		if (node.meshIndex >= 0 && node.meshIndex < static_cast<int>(meshes.size()))
		{
			auto* renderer = nodeObjects[i]->AddComponent<MeshRenderer>();
			renderer->mesh = &meshes[node.meshIndex];
		}
		if (node.skinnedMeshIndex >= 0 && node.skinnedMeshIndex < static_cast<int>(skinnedMeshes.size()))
		{
			auto* renderer = nodeObjects[i]->AddComponent<SkinnedMeshRenderer>();
			renderer->mesh = &skinnedMeshes[node.skinnedMeshIndex];
			renderer->animationController = animationController;
			renderer->skeleton = &skeleton;
		}
	}
}

void CubePrefab::Instantiate(GameObject& gameObject)
{
	ModelPrefab cubePrefab("model/Cube.glb");
	GameObject* cubeObject = gameObject.Instantiate(cubePrefab);
	gameObject.SetChild(*cubeObject);
}

void SpherePrefab::Instantiate(GameObject& gameObject)
{
	ModelPrefab cubePrefab("model/Sphere.glb");
	GameObject* cubeObject = gameObject.Instantiate(cubePrefab);
	gameObject.SetChild(*cubeObject);
}
