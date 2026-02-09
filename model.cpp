#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tiny_gltf.h"
#include "Model.h"

#include "DirectXTex.h"
#include "Renderer.h"
#include "DebugOstream.h"
#include <DirectXMath.h>
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <functional>
#include <unordered_set>
#include <wrl/client.h>

using namespace DirectX;

namespace
{
	DirectX::XMMATRIX ConvertMatrix(const DirectX::XMMATRIX& matrix)
	{
		const DirectX::XMMATRIX convert = DirectX::XMMatrixScaling(1.0f, 1.0f, -1.0f);
		return convert * matrix * convert;
	}

	DirectX::XMFLOAT3 ConvertVector(const DirectX::XMFLOAT3& value)
	{
		return { value.x, value.y, -value.z };
	}

	DirectX::XMFLOAT4 ConvertQuaternion(const DirectX::XMFLOAT4& value)
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

	DirectX::XMMATRIX MatrixFromGltf(const std::vector<double>& matrix)
	{
		return DirectX::XMMATRIX(
			static_cast<float>(matrix[0]), static_cast<float>(matrix[4]), static_cast<float>(matrix[8]), static_cast<float>(matrix[12]),
			static_cast<float>(matrix[1]), static_cast<float>(matrix[5]), static_cast<float>(matrix[9]), static_cast<float>(matrix[13]),
			static_cast<float>(matrix[2]), static_cast<float>(matrix[6]), static_cast<float>(matrix[10]), static_cast<float>(matrix[14]),
			static_cast<float>(matrix[3]), static_cast<float>(matrix[7]), static_cast<float>(matrix[11]), static_cast<float>(matrix[15])
		);
	}

	DirectX::XMMATRIX GetNodeLocalMatrix(const tinygltf::Node& node)
	{
		if (node.matrix.size() == 16)
		{
			return ConvertMatrix(MatrixFromGltf(node.matrix));
		}

		DirectX::XMFLOAT3 translation = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4 rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

		if (node.translation.size() == 3)
		{
			translation = { static_cast<float>(node.translation[0]), static_cast<float>(node.translation[1]), static_cast<float>(node.translation[2]) };
		}
		if (node.rotation.size() == 4)
		{
			rotation = { static_cast<float>(node.rotation[0]), static_cast<float>(node.rotation[1]), static_cast<float>(node.rotation[2]), static_cast<float>(node.rotation[3]) };
		}
		if (node.scale.size() == 3)
		{
			scale = { static_cast<float>(node.scale[0]), static_cast<float>(node.scale[1]), static_cast<float>(node.scale[2]) };
		}

		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w));
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		return ConvertMatrix(scaleMatrix * rotationMatrix * translationMatrix);
	}

	int ResolveTextureIndex(const tinygltf::Model& model, int materialIndex)
	{
		if (materialIndex < 0 || materialIndex >= static_cast<int>(model.materials.size()))
		{
			return -1;
		}

		const auto& material = model.materials[materialIndex];
		const int textureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
		if (textureIndex < 0 || textureIndex >= static_cast<int>(model.textures.size()))
		{
			return -1;
		}

		const auto& texture = model.textures[textureIndex];
		const int imageIndex = texture.source;
		if (imageIndex < 0 || imageIndex >= static_cast<int>(model.images.size()))
		{
			return -1;
		}

		return imageIndex;
	}

	DirectX::XMFLOAT4 ResolveBaseColorFactor(const tinygltf::Model& model, int materialIndex)
	{
		if (materialIndex < 0 || materialIndex >= static_cast<int>(model.materials.size()))
		{
			return { 1.0f, 1.0f, 1.0f, 1.0f };
		}

		const auto& factor = model.materials[materialIndex].pbrMetallicRoughness.baseColorFactor;
		if (factor.size() == 4)
		{
			return {
				static_cast<float>(factor[0]),
				static_cast<float>(factor[1]),
				static_cast<float>(factor[2]),
				static_cast<float>(factor[3])
			};
		}

		return { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	bool ReadAccessorFloats(const tinygltf::Model& model, int accessorIndex, std::vector<float>& out)
	{
		if (accessorIndex < 0 || accessorIndex >= static_cast<int>(model.accessors.size()))
		{
			return false;
		}
		const auto& accessor = model.accessors[accessorIndex];
		if (accessor.bufferView < 0 || accessor.bufferView >= static_cast<int>(model.bufferViews.size()))
		{
			return false;
		}
		const auto& bufferView = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[bufferView.buffer];
		if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
		{
			return false;
		}

		const size_t componentCount = tinygltf::GetNumComponentsInType(accessor.type);
		const size_t stride = accessor.ByteStride(bufferView) == 0
			? componentCount * sizeof(float)
			: accessor.ByteStride(bufferView);
		out.resize(accessor.count * componentCount);

		const unsigned char* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
		for (size_t i = 0; i < accessor.count; ++i)
		{
			const float* src = reinterpret_cast<const float*>(data + stride * i);
			for (size_t c = 0; c < componentCount; ++c)
			{
				out[i * componentCount + c] = src[c];
			}
		}

		return true;
	}

	bool ReadAccessorUInts(const tinygltf::Model& model, int accessorIndex, std::vector<UINT>& out)
	{
		if (accessorIndex < 0 || accessorIndex >= static_cast<int>(model.accessors.size()))
		{
			return false;
		}
		const auto& accessor = model.accessors[accessorIndex];
		if (accessor.bufferView < 0 || accessor.bufferView >= static_cast<int>(model.bufferViews.size()))
		{
			return false;
		}
		const auto& bufferView = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[bufferView.buffer];
		const size_t componentCount = tinygltf::GetNumComponentsInType(accessor.type);
		const size_t componentSize = tinygltf::GetComponentSizeInBytes(accessor.componentType);
		const size_t stride = accessor.ByteStride(bufferView) == 0
			? componentCount * componentSize
			: accessor.ByteStride(bufferView);
		out.resize(accessor.count * componentCount);

		const unsigned char* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
		for (size_t i = 0; i < accessor.count; ++i)
		{
			const unsigned char* src = data + stride * i;
			for (size_t c = 0; c < componentCount; ++c)
			{
				UINT value = 0;
				switch (accessor.componentType)
				{
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
					value = *(reinterpret_cast<const uint8_t*>(src + componentSize * c));
					break;
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					value = *(reinterpret_cast<const uint16_t*>(src + componentSize * c));
					break;
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
					value = *(reinterpret_cast<const uint32_t*>(src + componentSize * c));
					break;
				default:
					return false;
				}
				out[i * componentCount + c] = value;
			}
		}

		return true;
	}

	bool ReadAccessorMatrices(const tinygltf::Model& model, int accessorIndex, std::vector<DirectX::XMMATRIX>& out)
	{
		std::vector<float> values = {};
		if (!ReadAccessorFloats(model, accessorIndex, values))
		{
			return false;
		}
		const size_t count = values.size() / 16;
		out.resize(count);
		for (size_t i = 0; i < count; ++i)
		{
			const float* m = &values[i * 16];
			DirectX::XMMATRIX matrix = DirectX::XMMATRIX(
				m[0], m[4], m[8], m[12],
				m[1], m[5], m[9], m[13],
				m[2], m[6], m[10], m[14],
				m[3], m[7], m[11], m[15]
			);
			out[i] = ConvertMatrix(matrix);
		}
		return true;
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

bool Model::CreateBuffer(GraphicsDevice& device, const std::string& filePath)
{
	m_meshes.clear();
	m_skinnedMeshes.clear();
	m_textures.clear();
	m_nodes.clear();
	m_skeleton = {};
	m_animationClips.clear();

	m_baseDirectory = std::filesystem::path(filePath).parent_path();

	tinygltf::Model model = {};
	tinygltf::TinyGLTF loader = {};
	std::string warning = {};
	std::string error = {};
	const std::string extension = std::filesystem::path(filePath).extension().string();
	bool loaded = false;
	if (extension == ".glb" || extension == ".GLB")
	{
		loaded = loader.LoadBinaryFromFile(&model, &error, &warning, filePath);
	}
	else
	{
		loaded = loader.LoadASCIIFromFile(&model, &error, &warning, filePath);
	}

	if (!warning.empty())
	{
		hal::dout << warning << std::endl;
	}
	if (!loaded)
	{
		hal::dout << error << std::endl;
		return false;
	}

	LoadSkeleton(model);
	LoadTextures(device, model);

	int sceneIndex = model.defaultScene >= 0 ? model.defaultScene : 0;
	if (sceneIndex >= 0 && sceneIndex < static_cast<int>(model.scenes.size()))
	{
		const auto& scene = model.scenes[sceneIndex];
		for (int nodeIndex : scene.nodes)
		{
			ProcessNode(device, model, nodeIndex, -1);
		}
	}

	LoadAnimationClips(model);

	return true;
}

int Model::ProcessNode(GraphicsDevice& device, const tinygltf::Model& model, int nodeIndex, int parentIndex)
{
	if (nodeIndex < 0 || nodeIndex >= static_cast<int>(model.nodes.size()))
	{
		return -1;
	}

	const auto& node = model.nodes[nodeIndex];
	ModelNode modelNode = {};
	modelNode.name = node.name;
	modelNode.parent = parentIndex;
	modelNode.localMatrix = GetNodeLocalMatrix(node);

	int textureIndex = -1;

	if (node.mesh >= 0 && node.mesh < static_cast<int>(model.meshes.size()))
	{
		const auto& mesh = model.meshes[node.mesh];
		if (!mesh.primitives.empty())
		{
			textureIndex = ResolveTextureIndex(model, mesh.primitives.front().material);
			modelNode.baseColorFactor = ResolveBaseColorFactor(model, mesh.primitives.front().material);
		}

		if (node.skin >= 0)
		{
			const int skinnedMeshIndex = LoadSkinnedMesh(device, model, mesh.primitives.front(), node.skin);
			if (skinnedMeshIndex >= 0)
			{
				modelNode.skinnedMeshIndex = skinnedMeshIndex;
			}
		}
		else
		{
			const int meshIndex = LoadMesh(device, model, mesh.primitives.front());
			if (meshIndex >= 0)
			{
				modelNode.meshIndex = meshIndex;
			}
		}
	}

	if (textureIndex >= 0)
	{
		modelNode.textureIndex = textureIndex;
	}

	const int currentIndex = static_cast<int>(m_nodes.size());
	m_nodes.push_back(std::move(modelNode));

	for (int child : node.children)
	{
		const int childIndex = ProcessNode(device, model, child, currentIndex);
		if (childIndex >= 0)
		{
			m_nodes[currentIndex].children.push_back(childIndex);
		}
	}

	return currentIndex;
}

int Model::LoadMesh(GraphicsDevice& device, const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
	if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
	{
		return -1;
	}

	std::vector<float> positions = {};
	std::vector<float> normals = {};
	std::vector<float> uvs = {};
	const auto positionIt = primitive.attributes.find("POSITION");
	if (positionIt == primitive.attributes.end())
	{
		return -1;
	}

	ReadAccessorFloats(model, positionIt->second, positions);
	const size_t vertexCount = positions.size() / 3;
	const auto normalIt = primitive.attributes.find("NORMAL");
	if (normalIt != primitive.attributes.end())
	{
		ReadAccessorFloats(model, normalIt->second, normals);
	}
	const auto uvIt = primitive.attributes.find("TEXCOORD_0");
	if (uvIt != primitive.attributes.end())
	{
		ReadAccessorFloats(model, uvIt->second, uvs);
	}

	std::vector<Mesh::VertexAttribute> vertexes = {};
	vertexes.resize(vertexCount);
	for (size_t i = 0; i < vertexCount; ++i)
	{
		auto& vertex = vertexes[i];
		DirectX::XMFLOAT3 position = { positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2] };
		vertex.position = ConvertVector(position);
		vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (!normals.empty())
		{
			DirectX::XMFLOAT3 normal = { normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2] };
			vertex.normal = ConvertVector(normal);
		}
		if (uvs.size() >= (i + 1) * 2)
		{
			vertex.uv = { uvs[i * 2], uvs[i * 2 + 1] };
		}
	}

	std::vector<UINT> indexes = {};
	if (primitive.indices >= 0)
	{
		ReadAccessorUInts(model, primitive.indices, indexes);
	}
	else
	{
		indexes.resize(vertexCount);
		for (size_t i = 0; i < vertexCount; ++i)
		{
			indexes[i] = static_cast<UINT>(i);
		}
	}
	for (size_t i = 0; i + 2 < indexes.size(); i += 3)
	{
		std::swap(indexes[i + 1], indexes[i + 2]);
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

int Model::LoadSkinnedMesh(GraphicsDevice& device, const tinygltf::Model& model, const tinygltf::Primitive& primitive, int skinIndex)
{
	if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
	{
		return -1;
	}

	std::vector<float> positions = {};
	std::vector<float> normals = {};
	std::vector<float> uvs = {};
	std::vector<float> weights = {};
	std::vector<UINT> joints = {};

	const auto positionIt = primitive.attributes.find("POSITION");
	if (positionIt == primitive.attributes.end())
	{
		return -1;
	}
	ReadAccessorFloats(model, positionIt->second, positions);
	const size_t vertexCount = positions.size() / 3;

	const auto normalIt = primitive.attributes.find("NORMAL");
	if (normalIt != primitive.attributes.end())
	{
		ReadAccessorFloats(model, normalIt->second, normals);
	}
	const auto uvIt = primitive.attributes.find("TEXCOORD_0");
	if (uvIt != primitive.attributes.end())
	{
		ReadAccessorFloats(model, uvIt->second, uvs);
	}
	const auto weightIt = primitive.attributes.find("WEIGHTS_0");
	if (weightIt != primitive.attributes.end())
	{
		ReadAccessorFloats(model, weightIt->second, weights);
	}
	const auto jointsIt = primitive.attributes.find("JOINTS_0");
	if (jointsIt != primitive.attributes.end())
	{
		ReadAccessorUInts(model, jointsIt->second, joints);
	}

	std::vector<int> jointToBone = {};
	if (skinIndex >= 0 && skinIndex < static_cast<int>(model.skins.size()))
	{
		const auto& skin = model.skins[skinIndex];
		jointToBone.resize(skin.joints.size(), -1);
		for (size_t i = 0; i < skin.joints.size(); ++i)
		{
			const int jointNodeIndex = skin.joints[i];
			if (jointNodeIndex >= 0 && jointNodeIndex < static_cast<int>(model.nodes.size()))
			{
				const auto& jointNode = model.nodes[jointNodeIndex];
				jointToBone[i] = m_skeleton.FindBoneIndex(jointNode.name);
			}
		}
	}

	std::vector<SkinnedMesh::VertexAttribute> vertexes = {};
	vertexes.resize(vertexCount);
	for (size_t i = 0; i < vertexCount; ++i)
	{
		auto& vertex = vertexes[i];
		DirectX::XMFLOAT3 position = { positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2] };
		vertex.position = ConvertVector(position);
		vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (!normals.empty())
		{
			DirectX::XMFLOAT3 normal = { normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2] };
			vertex.normal = ConvertVector(normal);
		}
		if (uvs.size() >= (i + 1) * 2)
		{
			vertex.uv = { uvs[i * 2], uvs[i * 2 + 1] };
		}
		for (int k = 0; k < 4; ++k)
		{
			const size_t index = i * 4 + k;
			if (index < joints.size())
			{
				const UINT jointIndex = joints[index];
				if (jointIndex < jointToBone.size() && jointToBone[jointIndex] >= 0)
				{
					vertex.bone[k] = static_cast<UINT>(jointToBone[jointIndex]);
				}
			}
			if (index < weights.size())
			{
				vertex.weight[k] = weights[index];
			}
		}
	}

	std::vector<UINT> indexes = {};
	if (primitive.indices >= 0)
	{
		ReadAccessorUInts(model, primitive.indices, indexes);
	}
	else
	{
		indexes.resize(vertexCount);
		for (size_t i = 0; i < vertexCount; ++i)
		{
			indexes[i] = static_cast<UINT>(i);
		}
	}
	for (size_t i = 0; i + 2 < indexes.size(); i += 3)
	{
		std::swap(indexes[i + 1], indexes[i + 2]);
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

void Model::LoadTextures(GraphicsDevice& device, const tinygltf::Model& model)
{
	m_textures.clear();
	m_textures.resize(model.images.size());
	for (size_t imageIndex = 0; imageIndex < model.images.size(); ++imageIndex)
	{
		const auto& image = model.images[imageIndex];
		if (image.width <= 0 || image.height <= 0 || image.image.empty())
		{
			continue;
		}

		std::vector<uint8_t> rgba = {};
		if (image.component == 4)
		{
			rgba = image.image;
		}
		else if (image.component == 3)
		{
			rgba.resize(static_cast<size_t>(image.width) * image.height * 4);
			for (int i = 0; i < image.width * image.height; ++i)
			{
				rgba[i * 4] = image.image[i * 3];
				rgba[i * 4 + 1] = image.image[i * 3 + 1];
				rgba[i * 4 + 2] = image.image[i * 3 + 2];
				rgba[i * 4 + 3] = 255;
			}
		}
		else
		{
			rgba.resize(static_cast<size_t>(image.width) * image.height * 4);
			for (int i = 0; i < image.width * image.height; ++i)
			{
				const uint8_t value = image.image[i];
				rgba[i * 4] = value;
				rgba[i * 4 + 1] = value;
				rgba[i * 4 + 2] = value;
				rgba[i * 4 + 3] = 255;
			}
		}

		DirectX::ScratchImage scratch = {};
		if (FAILED(scratch.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, image.width, image.height, 1, 1)))
		{
			continue;
		}
		memcpy(scratch.GetPixels(), rgba.data(), rgba.size());

		DirectX::TexMetadata metadata = scratch.GetMetadata();
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;
		if (FAILED(DirectX::CreateShaderResourceView(
			device.GetDevice(),
			scratch.GetImages(),
			scratch.GetImageCount(),
			metadata,
			&srv)))
		{
			continue;
		}

		Texture texture = {};
		texture.CreateFromLoaded(device, srv.Detach(), static_cast<UINT>(metadata.width), static_cast<UINT>(metadata.height));
		m_textures[imageIndex] = std::move(texture);
	}
}

void Model::LoadSkeleton(const tinygltf::Model& model)
{
	std::unordered_set<int> jointNodes = {};
	std::unordered_map<int, DirectX::XMMATRIX> inverseBindMap = {};

	for (const auto& skin : model.skins)
	{
		for (int jointIndex : skin.joints)
		{
			jointNodes.insert(jointIndex);
		}
		if (skin.inverseBindMatrices >= 0)
		{
			std::vector<DirectX::XMMATRIX> matrices = {};
			if (ReadAccessorMatrices(model, skin.inverseBindMatrices, matrices))
			{
				const size_t count = std::min(matrices.size(), skin.joints.size());
				for (size_t i = 0; i < count; ++i)
				{
					inverseBindMap[skin.joints[i]] = matrices[i];
				}
			}
		}
	}

	std::function<void(int, int)> buildSkeleton = [&](int nodeIndex, int parentIndex)
	{
		if (nodeIndex < 0 || nodeIndex >= static_cast<int>(model.nodes.size()))
		{
			return;
		}

		const auto& node = model.nodes[nodeIndex];
		int currentIndex = parentIndex;
		if (jointNodes.find(nodeIndex) != jointNodes.end())
		{
			Bone bone = {};
			bone.name = node.name;
			bone.parentIndex = parentIndex;
			bone.offsetMatrix = DirectX::XMMatrixIdentity();
			bone.bindPose = GetNodeLocalMatrix(node);
			const auto inverseBindIt = inverseBindMap.find(nodeIndex);
			if (inverseBindIt != inverseBindMap.end())
			{
				bone.offsetMatrix = inverseBindIt->second;
			}
			m_skeleton.boneMap[bone.name] = static_cast<int>(m_skeleton.bones.size());
			m_skeleton.bones.push_back(std::move(bone));
			currentIndex = static_cast<int>(m_skeleton.bones.size()) - 1;
		}

		for (int child : node.children)
		{
			buildSkeleton(child, currentIndex);
		}
	};

	int sceneIndex = model.defaultScene >= 0 ? model.defaultScene : 0;
	if (sceneIndex >= 0 && sceneIndex < static_cast<int>(model.scenes.size()))
	{
		const auto& scene = model.scenes[sceneIndex];
		for (int nodeIndex : scene.nodes)
		{
			buildSkeleton(nodeIndex, -1);
		}
	}
}

void Model::LoadAnimationClips(const tinygltf::Model& model)
{
	for (const auto& animation : model.animations)
	{
		AnimationClip clip = {};
		clip.name = animation.name;
		clip.ticksPerSecond = 1.0f;
		clip.boneTracks.resize(m_skeleton.bones.size());
		float maxTime = 0.0f;

		for (const auto& channel : animation.channels)
		{
			if (channel.sampler < 0 || channel.sampler >= static_cast<int>(animation.samplers.size()))
			{
				continue;
			}
			if (channel.target_node < 0 || channel.target_node >= static_cast<int>(model.nodes.size()))
			{
				continue;
			}

			const auto& node = model.nodes[channel.target_node];
			const int boneIndex = m_skeleton.FindBoneIndex(node.name);
			if (boneIndex < 0)
			{
				continue;
			}

			const auto& sampler = animation.samplers[channel.sampler];
			std::vector<float> times = {};
			if (!ReadAccessorFloats(model, sampler.input, times))
			{
				continue;
			}

			BoneKeyframes& keyframes = clip.boneTracks[boneIndex];
			if (channel.target_path == "translation" || channel.target_path == "scale")
			{
				std::vector<float> values = {};
				if (!ReadAccessorFloats(model, sampler.output, values))
				{
					continue;
				}
				const size_t count = std::min(times.size(), values.size() / 3);
				for (size_t i = 0; i < count; ++i)
				{
					const DirectX::XMFLOAT3 value = { values[i * 3], values[i * 3 + 1], values[i * 3 + 2] };
					if (channel.target_path == "translation")
					{
						keyframes.positionKeyframes.push_back({ times[i], ConvertVector(value) });
					}
					else
					{
						keyframes.scaleKeyframes.push_back({ times[i], ConvertVector(value) });
					}
					maxTime = std::max(maxTime, times[i]);
				}
			}
			else if (channel.target_path == "rotation")
			{
				std::vector<float> values = {};
				if (!ReadAccessorFloats(model, sampler.output, values))
				{
					continue;
				}
				const size_t count = std::min(times.size(), values.size() / 4);
				for (size_t i = 0; i < count; ++i)
				{
					const DirectX::XMFLOAT4 value = { values[i * 4], values[i * 4 + 1], values[i * 4 + 2], values[i * 4 + 3] };
					keyframes.rotationKeyframes.push_back({ times[i], ConvertQuaternion(value) });
					maxTime = std::max(maxTime, times[i]);
				}
			}
		}

		clip.duration = maxTime;
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
	const auto& textures = model->GetTextures();
	std::vector<GameObject*> nodeObjects = CreateNodeObjects(gameObject, nodes);

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		const auto& node = nodes[i];
		if (node.meshIndex >= 0 && node.meshIndex < static_cast<int>(meshes.size()))
		{
			auto* renderer = nodeObjects[i]->AddComponent<MeshRenderer>();
			renderer->mesh = &meshes[node.meshIndex];
			renderer->material.SetColor(node.baseColorFactor);
			if (node.textureIndex >= 0 && node.textureIndex < static_cast<int>(textures.size()))
			{
				renderer->material.texture = &textures[node.textureIndex];
			}
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
	const auto& textures = model->GetTextures();
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
			renderer->material.SetColor(node.baseColorFactor);
			if (node.textureIndex >= 0 && node.textureIndex < static_cast<int>(textures.size()))
			{
				renderer->material.texture = &textures[node.textureIndex];
			}
		}
		if (node.skinnedMeshIndex >= 0 && node.skinnedMeshIndex < static_cast<int>(skinnedMeshes.size()))
		{
			auto* renderer = nodeObjects[i]->AddComponent<SkinnedMeshRenderer>();
			renderer->mesh = &skinnedMeshes[node.skinnedMeshIndex];
			renderer->animationController = animationController;
			renderer->skeleton = &skeleton;
			renderer->material.SetColor(node.baseColorFactor);
			if (node.textureIndex >= 0 && node.textureIndex < static_cast<int>(textures.size()))
			{
				renderer->material.texture = &textures[node.textureIndex];
			}
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
