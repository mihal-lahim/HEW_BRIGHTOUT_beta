#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tiny_gltf.h"

#include "Model.h"

#include "DirectXTex.h"
#include "Renderer.h"
#include <DirectXMath.h>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <unordered_set>
#include <wrl/client.h>

using namespace DirectX;

namespace
{
	std::string GetNodeName(const tinygltf::Node& node, int index)
	{
		if (!node.name.empty())
		{
			return node.name;
		}
		return "Node_" + std::to_string(index);
	}

	DirectX::XMMATRIX ConvertMatrix(const DirectX::XMMATRIX& matrix)
	{
		const DirectX::XMMATRIX convert = DirectX::XMMatrixScaling(1.0f, 1.0f, -1.0f);
		return convert * matrix * convert;
	}

	DirectX::XMFLOAT3 ConvertVector(const DirectX::XMFLOAT3& value)
	{
		return { value.x, value.y, -value.z };
	}

	DirectX::XMFLOAT3 ConvertScale(const DirectX::XMFLOAT3& value)
	{
		return { value.x, value.y, value.z };
	}

	DirectX::XMFLOAT4 ConvertQuaternion(const DirectX::XMFLOAT4& value)
	{
		return { value.x, value.y, -value.z, -value.w };
	}

	DirectX::XMMATRIX GetNodeLocalMatrix(const tinygltf::Node& node)
	{
		if (node.matrix.size() == 16)
		{
			const auto& m = node.matrix;

			DirectX::XMMATRIX matrix(
				static_cast<float>(m[0]), static_cast<float>(m[1]), static_cast<float>(m[2]), static_cast<float>(m[3]),
				static_cast<float>(m[4]), static_cast<float>(m[5]), static_cast<float>(m[6]), static_cast<float>(m[7]),
				static_cast<float>(m[8]), static_cast<float>(m[9]), static_cast<float>(m[10]), static_cast<float>(m[11]),
				static_cast<float>(m[12]), static_cast<float>(m[13]), static_cast<float>(m[14]), static_cast<float>(m[15])
			);
			return matrix;
		}

		DirectX::XMFLOAT3 translation = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT4 rotation = { 0.0f, 0.0f, 0.0f, 1.0f };

		if (node.translation.size() == 3)
		{
			translation = {
				static_cast<float>(node.translation[0]),
				static_cast<float>(node.translation[1]),
				static_cast<float>(node.translation[2])
			};
		}
		if (node.scale.size() == 3)
		{
			scale = {
				static_cast<float>(node.scale[0]),
				static_cast<float>(node.scale[1]),
				static_cast<float>(node.scale[2])
			};
		}
		if (node.rotation.size() == 4)
		{
			rotation = {
				static_cast<float>(node.rotation[0]),
				static_cast<float>(node.rotation[1]),
				static_cast<float>(node.rotation[2]),
				static_cast<float>(node.rotation[3])
			};
		}

		hal::dout << "rotation: " << rotation.x << ", " << rotation.y << ", " << rotation.z << ", " << rotation.w << "\n";


		const DirectX::XMVECTOR t = DirectX::XMLoadFloat3(&translation);
		const DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&scale);
		const DirectX::XMVECTOR r = DirectX::XMLoadFloat4(&rotation);

		XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
		XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation)); 
		XMMATRIX T = XMMatrixTranslation(translation.x, translation.y, translation.z);

		return S * R * T;
	}

	const unsigned char* GetAccessorData(const tinygltf::Model& model, const tinygltf::Accessor& accessor, size_t& stride)
	{
		const auto& view = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[view.buffer];
		stride = accessor.ByteStride(view);
		if (stride == 0)
		{
			stride = tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type);
		}
		return buffer.data.data() + view.byteOffset + accessor.byteOffset;
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
	m_meshes.clear();
	m_skinnedMeshes.clear();
	m_textures.clear();
	m_nodes.clear();
	m_skeleton = {};
	m_animationClips.clear();

	m_baseDirectory = std::filesystem::path(filePath).parent_path();

	tinygltf::Model gltfModel = {};
	tinygltf::TinyGLTF loader = {};
	std::string err = {};
	std::string warn = {};
	const bool loaded = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, filePath);
	if (!warn.empty())
	{
		hal::dout << warn << std::endl;
	}
	if (!err.empty())
	{
		hal::dout << err << std::endl;
	}
	if (!loaded)
	{
		return false;
	}

	LoadSkeleton(gltfModel);

	int sceneIndex = gltfModel.defaultScene;
	if (sceneIndex < 0 && !gltfModel.scenes.empty())
	{
		sceneIndex = 0;
	}
	if (sceneIndex >= 0 && sceneIndex < static_cast<int>(gltfModel.scenes.size()))
	{
		const auto& scene = gltfModel.scenes[sceneIndex];
		for (int nodeIndex : scene.nodes)
		{
			ProcessNode(device, gltfModel, nodeIndex, -1);
		}
	}

	LoadTextures(device, gltfModel);
	LoadAnimationClips(gltfModel);

	return true;
}

int Model::ProcessNode(GraphicsDevice& device, const tinygltf::Model& model, int nodeIndex, int parentIndex)
{
	const auto& node = model.nodes[nodeIndex];
	ModelNode modelNode = {};
	modelNode.name = GetNodeName(node, nodeIndex);
	modelNode.parent = parentIndex;
	modelNode.localMatrix = ConvertMatrix(GetNodeLocalMatrix(node));

	const int modelNodeIndex = static_cast<int>(m_nodes.size());
	m_nodes.push_back(std::move(modelNode));

	if (node.mesh >= 0 && node.mesh < static_cast<int>(model.meshes.size()))
	{
		const auto& mesh = model.meshes[node.mesh];
		const bool isSkinned = node.skin >= 0;
		const size_t primitiveCount = mesh.primitives.size();
		for (size_t i = 0; i < primitiveCount; ++i)
		{
			const auto& primitive = mesh.primitives[i];
			int meshIndex = -1;
			if (isSkinned)
			{
				meshIndex = LoadSkinnedMesh(device, model, primitive, node.skin);
			}
			else
			{
				meshIndex = LoadMesh(device, model, primitive);
			}

			DirectX::XMFLOAT4 baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			int textureIndex = -1;
			if (primitive.material >= 0 && primitive.material < static_cast<int>(model.materials.size()))
			{
				const auto& material = model.materials[primitive.material];
				const auto& pbr = material.pbrMetallicRoughness;
				if (pbr.baseColorFactor.size() == 4)
				{
					baseColor = {
						static_cast<float>(pbr.baseColorFactor[0]),
						static_cast<float>(pbr.baseColorFactor[1]),
						static_cast<float>(pbr.baseColorFactor[2]),
						static_cast<float>(pbr.baseColorFactor[3])
					};
				}
				if (pbr.baseColorTexture.index >= 0 && pbr.baseColorTexture.index < static_cast<int>(model.textures.size()))
				{
					const auto& texture = model.textures[pbr.baseColorTexture.index];
					textureIndex = texture.source;
				}
			}

			if (primitiveCount == 1)
			{
				if (isSkinned)
				{
					m_nodes[modelNodeIndex].skinnedMeshIndex = meshIndex;
				}
				else
				{
					m_nodes[modelNodeIndex].meshIndex = meshIndex;
				}
				m_nodes[modelNodeIndex].textureIndex = textureIndex;
				m_nodes[modelNodeIndex].baseColorFactor = baseColor;
			}
			else
			{
				ModelNode childNode = {};
				childNode.name = modelNode.name + "_Primitive" + std::to_string(i);
				childNode.parent = modelNodeIndex;
				childNode.localMatrix = DirectX::XMMatrixIdentity();
				childNode.textureIndex = textureIndex;
				childNode.baseColorFactor = baseColor;
				if (isSkinned)
				{
					childNode.skinnedMeshIndex = meshIndex;
				}
				else
				{
					childNode.meshIndex = meshIndex;
				}
				const int childIndex = static_cast<int>(m_nodes.size());
				m_nodes.push_back(std::move(childNode));
				m_nodes[modelNodeIndex].children.push_back(childIndex);
			}
		}
	}

	for (int childNodeIndex : node.children)
	{
		const int childIndex = ProcessNode(device, model, childNodeIndex, modelNodeIndex);
		m_nodes[modelNodeIndex].children.push_back(childIndex);
	}

	return modelNodeIndex;
}

int Model::LoadMesh(GraphicsDevice& device, const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
	if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
	{
		return -1;
	}

	auto positionIt = primitive.attributes.find("POSITION");
	if (positionIt == primitive.attributes.end())
	{
		return -1;
	}

	const tinygltf::Accessor& positionAccessor = model.accessors[positionIt->second];
	std::vector<Mesh::VertexAttribute> vertexes = {};
	vertexes.resize(positionAccessor.count);

	size_t positionStride = 0;
	const unsigned char* positionData = GetAccessorData(model, positionAccessor, positionStride);

	const tinygltf::Accessor* normalAccessor = nullptr;
	const unsigned char* normalData = nullptr;
	size_t normalStride = 0;
	if (auto normalIt = primitive.attributes.find("NORMAL"); normalIt != primitive.attributes.end())
	{
		normalAccessor = &model.accessors[normalIt->second];
		normalData = GetAccessorData(model, *normalAccessor, normalStride);
	}

	const tinygltf::Accessor* texcoordAccessor = nullptr;
	const unsigned char* texcoordData = nullptr;
	size_t texcoordStride = 0;
	if (auto texcoordIt = primitive.attributes.find("TEXCOORD_0"); texcoordIt != primitive.attributes.end())
	{
		texcoordAccessor = &model.accessors[texcoordIt->second];
		texcoordData = GetAccessorData(model, *texcoordAccessor, texcoordStride);
	}

	for (size_t i = 0; i < vertexes.size(); ++i)
	{
		auto& vertex = vertexes[i];
		const float* position = reinterpret_cast<const float*>(positionData + i * positionStride);
		vertex.position = ConvertVector({ position[0], position[1], position[2] });
		vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (normalAccessor)
		{
			const float* normal = reinterpret_cast<const float*>(normalData + i * normalStride);
			vertex.normal = ConvertVector({ normal[0], normal[1], normal[2] });
		}
		if (texcoordAccessor)
		{
			const float* uv = reinterpret_cast<const float*>(texcoordData + i * texcoordStride);
			vertex.uv = { uv[0], uv[1] };
		}
	}

	std::vector<UINT> indexes = {};
	if (primitive.indices >= 0)
	{
		const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
		size_t indexStride = 0;
		const unsigned char* indexData = GetAccessorData(model, indexAccessor, indexStride);
		std::vector<UINT> rawIndices = {};
		rawIndices.resize(indexAccessor.count);

		for (size_t i = 0; i < indexAccessor.count; ++i)
		{
			const unsigned char* ptr = indexData + i * indexStride;
			switch (indexAccessor.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				rawIndices[i] = static_cast<UINT>(*ptr);
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				rawIndices[i] = static_cast<UINT>(*reinterpret_cast<const uint16_t*>(ptr));
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				rawIndices[i] = static_cast<UINT>(*reinterpret_cast<const uint32_t*>(ptr));
				break;
			default:
				rawIndices[i] = 0;
				break;
			}
		}

		indexes.reserve(rawIndices.size());
		for (size_t i = 0; i + 2 < rawIndices.size(); i += 3)
		{
			indexes.push_back(rawIndices[i]);
			indexes.push_back(rawIndices[i + 2]);
			indexes.push_back(rawIndices[i + 1]);
		}
	}
	else
	{
		indexes.reserve(vertexes.size());
		for (UINT i = 0; i + 2 < static_cast<UINT>(vertexes.size()); i += 3)
		{
			indexes.push_back(i);
			indexes.push_back(i + 2);
			indexes.push_back(i + 1);
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

int Model::LoadSkinnedMesh(GraphicsDevice& device, const tinygltf::Model& model, const tinygltf::Primitive& primitive, int skinIndex)
{
	if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
	{
		return -1;
	}
	if (skinIndex < 0 || skinIndex >= static_cast<int>(model.skins.size()))
	{
		return -1;
	}

	auto positionIt = primitive.attributes.find("POSITION");
	if (positionIt == primitive.attributes.end())
	{
		return -1;
	}

	const tinygltf::Accessor& positionAccessor = model.accessors[positionIt->second];
	std::vector<SkinnedMesh::VertexAttribute> vertexes = {};
	vertexes.resize(positionAccessor.count);

	size_t positionStride = 0;
	const unsigned char* positionData = GetAccessorData(model, positionAccessor, positionStride);

	const tinygltf::Accessor* normalAccessor = nullptr;
	const unsigned char* normalData = nullptr;
	size_t normalStride = 0;
	if (auto normalIt = primitive.attributes.find("NORMAL"); normalIt != primitive.attributes.end())
	{
		normalAccessor = &model.accessors[normalIt->second];
		normalData = GetAccessorData(model, *normalAccessor, normalStride);
	}

	const tinygltf::Accessor* texcoordAccessor = nullptr;
	const unsigned char* texcoordData = nullptr;
	size_t texcoordStride = 0;
	if (auto texcoordIt = primitive.attributes.find("TEXCOORD_0"); texcoordIt != primitive.attributes.end())
	{
		texcoordAccessor = &model.accessors[texcoordIt->second];
		texcoordData = GetAccessorData(model, *texcoordAccessor, texcoordStride);
	}

	const tinygltf::Accessor* jointsAccessor = nullptr;
	const unsigned char* jointsData = nullptr;
	size_t jointsStride = 0;
	if (auto jointsIt = primitive.attributes.find("JOINTS_0"); jointsIt != primitive.attributes.end())
	{
		jointsAccessor = &model.accessors[jointsIt->second];
		jointsData = GetAccessorData(model, *jointsAccessor, jointsStride);
	}

	const tinygltf::Accessor* weightsAccessor = nullptr;
	const unsigned char* weightsData = nullptr;
	size_t weightsStride = 0;
	if (auto weightsIt = primitive.attributes.find("WEIGHTS_0"); weightsIt != primitive.attributes.end())
	{
		weightsAccessor = &model.accessors[weightsIt->second];
		weightsData = GetAccessorData(model, *weightsAccessor, weightsStride);
	}

	const auto& skin = model.skins[skinIndex];

	for (size_t i = 0; i < vertexes.size(); ++i)
	{
		auto& vertex = vertexes[i];
		const float* position = reinterpret_cast<const float*>(positionData + i * positionStride);
		vertex.position = ConvertVector({ position[0], position[1], position[2] });
		vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (normalAccessor)
		{
			const float* normal = reinterpret_cast<const float*>(normalData + i * normalStride);
			vertex.normal = ConvertVector({ normal[0], normal[1], normal[2] });
		}
		if (texcoordAccessor)
		{
			const float* uv = reinterpret_cast<const float*>(texcoordData + i * texcoordStride);
			vertex.uv = { uv[0], uv[1] };
		}

		if (jointsAccessor && weightsAccessor)
		{
			const unsigned char* jointPtr = jointsData + i * jointsStride;
			const unsigned char* weightPtr = weightsData + i * weightsStride;
			for (int k = 0; k < 4; ++k)
			{
				uint16_t jointIndex = 0;
				switch (jointsAccessor->componentType)
				{
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
					jointIndex = static_cast<uint16_t>(jointPtr[k]);
					break;
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					jointIndex = reinterpret_cast<const uint16_t*>(jointPtr)[k];
					break;
				default:
					jointIndex = 0;
					break;
				}

				int boneIndex = 0;
				if (jointIndex < skin.joints.size())
				{
					const int jointNodeIndex = skin.joints[jointIndex];
					const auto& jointNode = model.nodes[jointNodeIndex];
					const std::string jointName = GetNodeName(jointNode, jointNodeIndex);
					const int foundIndex = m_skeleton.FindBoneIndex(jointName);
					if (foundIndex >= 0)
					{
						boneIndex = foundIndex;
					}
				}
				vertex.bone[k] = static_cast<UINT>(boneIndex);

				float weightValue = 0.0f;
				switch (weightsAccessor->componentType)
				{
				case TINYGLTF_COMPONENT_TYPE_FLOAT:
					weightValue = reinterpret_cast<const float*>(weightPtr)[k];
					break;
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
					weightValue = static_cast<float>(weightPtr[k]) / 255.0f;
					break;
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					weightValue = static_cast<float>(reinterpret_cast<const uint16_t*>(weightPtr)[k]) / 65535.0f;
					break;
				default:
					weightValue = 0.0f;
					break;
				}
				vertex.weight[k] = weightValue;
			}
		}
	}

	std::vector<UINT> indexes = {};
	if (primitive.indices >= 0)
	{
		const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
		size_t indexStride = 0;
		const unsigned char* indexData = GetAccessorData(model, indexAccessor, indexStride);
		std::vector<UINT> rawIndices = {};
		rawIndices.resize(indexAccessor.count);

		for (size_t i = 0; i < indexAccessor.count; ++i)
		{
			const unsigned char* ptr = indexData + i * indexStride;
			switch (indexAccessor.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				rawIndices[i] = static_cast<UINT>(*ptr);
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				rawIndices[i] = static_cast<UINT>(*reinterpret_cast<const uint16_t*>(ptr));
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				rawIndices[i] = static_cast<UINT>(*reinterpret_cast<const uint32_t*>(ptr));
				break;
			default:
				rawIndices[i] = 0;
				break;
			}
		}

		indexes.reserve(rawIndices.size());
		for (size_t i = 0; i + 2 < rawIndices.size(); i += 3)
		{
			indexes.push_back(rawIndices[i]);
			indexes.push_back(rawIndices[i + 2]);
			indexes.push_back(rawIndices[i + 1]);
		}
	}
	else
	{
		indexes.reserve(vertexes.size());
		for (UINT i = 0; i + 2 < static_cast<UINT>(vertexes.size()); i += 3)
		{
			indexes.push_back(i);
			indexes.push_back(i + 2);
			indexes.push_back(i + 1);
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

void Model::LoadTextures(GraphicsDevice& device, const tinygltf::Model& model)
{
	for (const auto& image : model.images)
	{
		if (image.image.empty() || image.width <= 0 || image.height <= 0)
		{
			m_textures.emplace_back();
			continue;
		}

		const UINT width = static_cast<UINT>(image.width);
		const UINT height = static_cast<UINT>(image.height);
		DirectX::ScratchImage scratch = {};
		if (FAILED(scratch.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
		{
			m_textures.emplace_back();
			continue;
		}

		const DirectX::Image* dstImage = scratch.GetImage(0, 0, 0);
		uint8_t* dst = dstImage->pixels;
		const size_t dstRowPitch = dstImage->rowPitch;
		const uint8_t* src = image.image.data();
		const int components = image.component > 0 ? image.component : 4;
		for (UINT y = 0; y < height; ++y)
		{
			uint8_t* row = dst + y * dstRowPitch;
			const uint8_t* srcRow = src + static_cast<size_t>(y) * width * components;
			for (UINT x = 0; x < width; ++x)
			{
				const uint8_t* pixel = srcRow + static_cast<size_t>(x) * components;
				row[x * 4 + 0] = pixel[0];
				row[x * 4 + 1] = components > 1 ? pixel[1] : pixel[0];
				row[x * 4 + 2] = components > 2 ? pixel[2] : pixel[0];
				row[x * 4 + 3] = components > 3 ? pixel[3] : 255;
			}
		}

		DirectX::TexMetadata metadata = scratch.GetMetadata();
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;
		if (FAILED(DirectX::CreateShaderResourceView(
			device.GetDevice(),
			scratch.GetImages(),
			scratch.GetImageCount(),
			metadata,
			&srv)))
		{
			m_textures.emplace_back();
			continue;
		}

		Texture texture = {};
		texture.CreateFromLoaded(device, srv.Detach(), width, height);
		m_textures.push_back(std::move(texture));
	}
}

void Model::LoadSkeleton(const tinygltf::Model& model)
{
	if (model.skins.empty())
	{
		return;
	}

	std::vector<int> parentMap(model.nodes.size(), -1);
	for (size_t i = 0; i < model.nodes.size(); ++i)
	{
		for (int childIndex : model.nodes[i].children)
		{
			if (childIndex >= 0 && childIndex < static_cast<int>(parentMap.size()))
			{
				parentMap[childIndex] = static_cast<int>(i);
			}
		}
	}

	std::vector<int> orderedJoints = {};
	std::unordered_set<int> jointSet = {};
	std::unordered_map<int, DirectX::XMMATRIX> inverseBindMap = {};

	for (const auto& skin : model.skins)
	{
		if (skin.inverseBindMatrices >= 0 && skin.inverseBindMatrices < static_cast<int>(model.accessors.size()))
		{
			const auto& accessor = model.accessors[skin.inverseBindMatrices];
			size_t stride = 0;
			const unsigned char* data = GetAccessorData(model, accessor, stride);
			for (size_t i = 0; i < skin.joints.size(); ++i)
			{
				const float* matrixData = reinterpret_cast<const float*>(data + i * stride);
				DirectX::XMMATRIX matrix(
					matrixData[0], matrixData[1], matrixData[2], matrixData[3],
					matrixData[4], matrixData[5], matrixData[6], matrixData[7],
					matrixData[8], matrixData[9], matrixData[10], matrixData[11],
					matrixData[12], matrixData[13], matrixData[14], matrixData[15]);
				matrix = DirectX::XMMatrixTranspose(matrix);
				matrix = ConvertMatrix(matrix);
				inverseBindMap[skin.joints[i]] = matrix;
			}
		}

		for (int jointIndex : skin.joints)
		{
			if (jointSet.insert(jointIndex).second)
			{
				orderedJoints.push_back(jointIndex);
			}
		}
	}

	std::unordered_map<int, int> nodeToBone = {};
	for (int jointIndex : orderedJoints)
	{
		if (jointIndex < 0 || jointIndex >= static_cast<int>(model.nodes.size()))
		{
			continue;
		}

		const auto& node = model.nodes[jointIndex];
		Bone bone = {};
		bone.name = GetNodeName(node, jointIndex);
		bone.parentIndex = -1;
		bone.bindPose = ConvertMatrix(GetNodeLocalMatrix(node));
		bone.offsetMatrix = XMMatrixInverse(nullptr, bone.bindPose);

		
		/*
		auto it = inverseBindMap.find(jointIndex);
		if (it != inverseBindMap.end())
		{
			bone.offsetMatrix = it->second;
		}
		*/
		


		hal::dout << "Bone: " << bone.name << "\n"; 
		
		// bindPose 出力 
		hal::dout << "bindPose:\n"; 
		for (int row = 0; row < 4; ++row) 
		{ 
			XMFLOAT4 r; 
			XMStoreFloat4(&r, bone.bindPose.r[row]); 
			hal::dout << r.x << " " << r.y << " " << r.z << " " << r.w << "\n"; 
		}

		// offsetMatrix 出力（inverseBindMatrix） 
		hal::dout << "offsetMatrix:\n"; 

		for (int row = 0; row < 4; ++row) 
		{ 
			XMFLOAT4 r; 
			XMStoreFloat4(&r, bone.offsetMatrix.r[row]); 
			hal::dout << r.x << " " << r.y << " " << r.z << " " << r.w << "\n"; 
		} 

		// bindPose * offsetMatrix の積（単位行列に近いか？） 
		XMMATRIX product = bone.bindPose * bone.offsetMatrix; 
		hal::dout << "bindPose * offsetMatrix:\n"; 
		for (int row = 0; row < 4; ++row) 
		{ 
			XMFLOAT4 r; 
			XMStoreFloat4(&r, product.r[row]); 
			hal::dout << r.x << " " << r.y << " " << r.z << " " << r.w << "\n";
		}

		const int boneIndex = static_cast<int>(m_skeleton.bones.size());
		m_skeleton.boneMap[bone.name] = boneIndex;
		m_skeleton.bones.push_back(std::move(bone));
		nodeToBone[jointIndex] = boneIndex;
	}

	for (const auto& [nodeIndex, boneIndex] : nodeToBone)
	{
		int parentIndex = -1;
		if (nodeIndex >= 0 && nodeIndex < static_cast<int>(parentMap.size()))
		{
			const int parentNode = parentMap[nodeIndex];
			if (auto it = nodeToBone.find(parentNode); it != nodeToBone.end())
			{
				parentIndex = it->second;
			}
		}
		m_skeleton.bones[boneIndex].parentIndex = parentIndex;
	}
}

void Model::LoadAnimationClips(const tinygltf::Model& model)
{
	for (size_t i = 0; i < model.animations.size(); ++i)
	{
		const auto& animation = model.animations[i];
		AnimationClip clip = {};
		clip.name = animation.name.empty() ? "Animation_" + std::to_string(i) : animation.name;
		clip.ticksPerSecond = 1.0f;
		clip.boneTracks.resize(m_skeleton.bones.size());

		float maxTime = 0.0f;
		for (const auto& channel : animation.channels)
		{
			if (channel.target_node < 0 || channel.target_node >= static_cast<int>(model.nodes.size()))
			{
				continue;
			}

			const auto& targetNode = model.nodes[channel.target_node];
			const std::string targetName = GetNodeName(targetNode, channel.target_node);
			const int boneIndex = m_skeleton.FindBoneIndex(targetName);
			if (boneIndex < 0 || channel.sampler < 0 || channel.sampler >= static_cast<int>(animation.samplers.size()))
			{
				continue;
			}

			const auto& sampler = animation.samplers[channel.sampler];
			if (sampler.input < 0 || sampler.input >= static_cast<int>(model.accessors.size()) ||
				sampler.output < 0 || sampler.output >= static_cast<int>(model.accessors.size()))
			{
				continue;
			}

			const auto& inputAccessor = model.accessors[sampler.input];
			const auto& outputAccessor = model.accessors[sampler.output];
			size_t inputStride = 0;
			size_t outputStride = 0;
			const unsigned char* inputData = GetAccessorData(model, inputAccessor, inputStride);
			const unsigned char* outputData = GetAccessorData(model, outputAccessor, outputStride);
			const size_t keyCount = std::min(inputAccessor.count, outputAccessor.count);
			BoneKeyframes& keyframes = clip.boneTracks[boneIndex];

			for (size_t k = 0; k < keyCount; ++k)
			{
				float time = 0.0f;
				const unsigned char* timePtr = inputData + k * inputStride;
				switch (inputAccessor.componentType)
				{
				case TINYGLTF_COMPONENT_TYPE_FLOAT:
					time = *reinterpret_cast<const float*>(timePtr);
					break;
				case TINYGLTF_COMPONENT_TYPE_DOUBLE:
					time = static_cast<float>(*reinterpret_cast<const double*>(timePtr));
					break;
				default:
					time = 0.0f;
					break;
				}

				maxTime = std::max(maxTime, time);
				const unsigned char* valuePtr = outputData + k * outputStride;
				if (channel.target_path == "translation" && outputAccessor.type == TINYGLTF_TYPE_VEC3)
				{
					const float* value = reinterpret_cast<const float*>(valuePtr);
					keyframes.positionKeyframes.push_back({ time, ConvertVector({ value[0], value[1], value[2] }) });
				}
				else if (channel.target_path == "scale" && outputAccessor.type == TINYGLTF_TYPE_VEC3)
				{
					const float* value = reinterpret_cast<const float*>(valuePtr);
					keyframes.scaleKeyframes.push_back({ time, ConvertScale({ value[0], value[1], value[2] }) });
				}
				else if (channel.target_path == "rotation" && outputAccessor.type == TINYGLTF_TYPE_VEC4)
				{
					const float* value = reinterpret_cast<const float*>(valuePtr);
					keyframes.rotationKeyframes.push_back({ time, ConvertQuaternion({ value[0], value[1], value[2], value[3] }) });
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
			renderer->model = model;
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
			renderer->model = model;
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
