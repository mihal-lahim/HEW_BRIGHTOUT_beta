#include "Material.h"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <vector>

namespace
{
	UINT CalculateConstantBufferSize(const ConstantBufferInfo& bufferInfo)
	{
		if (bufferInfo.size != 0)
		{
			return static_cast<UINT>(bufferInfo.size);
		}

		size_t maxSize = 0;
		for (const auto& [name, info] : bufferInfo.variables)
		{
			maxSize = std::max(maxSize, info.offset + info.size);
		}
		return static_cast<UINT>(maxSize);
	}
}

bool Material::CreateBuffer(GraphicsDevice& device, ShaderProgram* _shaderProgram)
{
	if (!_shaderProgram)
	{
		return false;
	}

	shaderProgram = _shaderProgram;
	auto bufferIt = shaderProgram->mergedReflectionInfo.buffers.find("PER_MATERIAL");
	if (bufferIt == shaderProgram->mergedReflectionInfo.buffers.end())
	{
		return false;
	}

	UINT bufferSize = CalculateConstantBufferSize(bufferIt->second);
	if (bufferSize == 0)
	{
		return false;
	}

	m_isDirty = true;
	return materialConstantBuffer.CreateBuffer(device, bufferSize);
}

void Material::Apply(GraphicsDevice& device)
{
	if (!m_isDirty || !shaderProgram)
	{
		return;
	}

	auto bufferIt = shaderProgram->mergedReflectionInfo.buffers.find("PER_MATERIAL");
	if (bufferIt == shaderProgram->mergedReflectionInfo.buffers.end())
	{
		return;
	}

	const auto& bufferInfo = bufferIt->second;
	UINT bufferSize = CalculateConstantBufferSize(bufferInfo);
	if (bufferSize == 0)
	{
		return;
	}

	std::vector<std::byte> bufferData(bufferSize, std::byte{ 0 });
	for (const auto& [name, info] : bufferInfo.variables)
	{
		auto* dest = bufferData.data() + info.offset;
		switch (info.type)
		{
		case ShaderValueType::Float:
		{
			auto it = m_floatParams.find(name);
			if (it != m_floatParams.end())
			{
				std::memcpy(dest, &it->second, sizeof(it->second));
			}
			break;
		}
		case ShaderValueType::Float4:
		{
			auto it = m_float4Params.find(name);
			if (it != m_float4Params.end())
			{
				std::memcpy(dest, &it->second, sizeof(it->second));
			}
			break;
		}
		case ShaderValueType::Matrix4x4:
		{
			auto it = m_matrix4x4Params.find(name);
			if (it != m_matrix4x4Params.end())
			{
				DirectX::XMFLOAT4X4 value{};
				DirectX::XMStoreFloat4x4(&value, DirectX::XMMatrixTranspose(it->second));
				std::memcpy(dest, &value, sizeof(value));
			}
			break;
		}
		default:
			break;
		}
	}

	materialConstantBuffer.UpdateBuffer(device, bufferData.data(), bufferSize);
	m_isDirty = false;
}

void Material::Bind(GraphicsDevice& device)
{
	Apply(device);
	materialConstantBuffer.BindVS(device, 3);
	materialConstantBuffer.BindPS(device, 3);
}
