#include "GraphicsDevice.h"
#include "ResourceSystem.h"
#include "Shader.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <Windows.h>

namespace
{
	std::wstring ToWideString(const std::string& value)
	{
		return std::wstring(value.begin(), value.end());
	}

	bool LoadShaderBlob(const std::string& filePath, const char* target, Microsoft::WRL::ComPtr<ID3DBlob>& blob)
	{
		auto widePath = ToWideString(filePath);
		if (SUCCEEDED(D3DReadFileToBlob(widePath.c_str(), blob.GetAddressOf())))
		{
			return true;
		}

		auto dotPos = filePath.find_last_of('.');
		if (dotPos == std::string::npos)
		{
			return false;
		}

		std::string extension = filePath.substr(dotPos);
		std::transform(extension.begin(), extension.end(), extension.begin(),
			[](unsigned char c) { return static_cast<char>(::tolower(c)); });

		std::wstring hlslPath;
		if (extension == ".cso")
		{
			hlslPath = ToWideString(filePath.substr(0, dotPos) + ".hlsl");
		}
		else if (extension == ".hlsl")
		{
			hlslPath = widePath;
		}
		else
		{
			return false;
		}

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3DCompileFromFile(hlslPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
			target, flags, 0, blob.GetAddressOf(), errorBlob.GetAddressOf());
		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
			}
			return false;
		}
		return true;
	}

	bool CreateShaderReflection(ID3DBlob* blob, Microsoft::WRL::ComPtr<ID3D11ShaderReflection>& reflection)
	{
		return SUCCEEDED(D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection,
			reinterpret_cast<void**>(reflection.GetAddressOf())));
	}

	ShaderValueType GetShaderValueType(const D3D11_SHADER_TYPE_DESC& typeDesc)
	{
		if (typeDesc.Type != D3D_SVT_FLOAT)
		{
			return ShaderValueType::None;
		}

		if (typeDesc.Rows == 1 && typeDesc.Columns == 1)
		{
			return ShaderValueType::Float;
		}
		if (typeDesc.Rows == 1 && typeDesc.Columns == 4)
		{
			return ShaderValueType::Float4;
		}
		if (typeDesc.Rows == 4 && typeDesc.Columns == 4)
		{
			return ShaderValueType::Matrix4x4;
		}
		return ShaderValueType::None;
	}

	void BuildShaderReflectionInfo(ID3D11ShaderReflection* reflection, ShaderReflectionInfo& info)
	{
		D3D11_SHADER_DESC shaderDesc{};
		reflection->GetDesc(&shaderDesc);

		for (UINT bufferIndex = 0; bufferIndex < shaderDesc.ConstantBuffers; ++bufferIndex)
		{
			auto* constantBuffer = reflection->GetConstantBufferByIndex(bufferIndex);
			if (!constantBuffer)
			{
				continue;
			}

			D3D11_SHADER_BUFFER_DESC bufferDesc{};
			constantBuffer->GetDesc(&bufferDesc);
			if (!bufferDesc.Name)
			{
				continue;
			}

			ConstantBufferInfo bufferInfo{};
			bufferInfo.size = bufferDesc.Size;

			for (UINT varIndex = 0; varIndex < bufferDesc.Variables; ++varIndex)
			{
				auto* variable = constantBuffer->GetVariableByIndex(varIndex);
				if (!variable)
				{
					continue;
				}

				D3D11_SHADER_VARIABLE_DESC varDesc{};
				variable->GetDesc(&varDesc);
				auto* type = variable->GetType();
				D3D11_SHADER_TYPE_DESC typeDesc{};
				if (type)
				{
					type->GetDesc(&typeDesc);
				}

				ConstantBufferVariableInfo varInfo{};
				if (varDesc.Name)
				{
					varInfo.name = varDesc.Name;
				}
				varInfo.offset = varDesc.StartOffset;
				varInfo.size = varDesc.Size;
				varInfo.type = GetShaderValueType(typeDesc);
				info.constantBuffers[varInfo.name] = varInfo;
				bufferInfo.variables[varInfo.name] = varInfo;
			}

			info.buffers[bufferDesc.Name] = bufferInfo;
		}
	}

	DXGI_FORMAT GetInputFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc)
	{
		if (paramDesc.Mask == 1)
		{
			switch (paramDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				return DXGI_FORMAT_R32_UINT;
			case D3D_REGISTER_COMPONENT_SINT32:
				return DXGI_FORMAT_R32_SINT;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32_FLOAT;
			default:
				return DXGI_FORMAT_UNKNOWN;
			}
		}
		if (paramDesc.Mask <= 3)
		{
			switch (paramDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				return DXGI_FORMAT_R32G32_UINT;
			case D3D_REGISTER_COMPONENT_SINT32:
				return DXGI_FORMAT_R32G32_SINT;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32_FLOAT;
			default:
				return DXGI_FORMAT_UNKNOWN;
			}
		}
		if (paramDesc.Mask <= 7)
		{
			switch (paramDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				return DXGI_FORMAT_R32G32B32_UINT;
			case D3D_REGISTER_COMPONENT_SINT32:
				return DXGI_FORMAT_R32G32B32_SINT;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32B32_FLOAT;
			default:
				return DXGI_FORMAT_UNKNOWN;
			}
		}
		if (paramDesc.Mask <= 15)
		{
			switch (paramDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				return DXGI_FORMAT_R32G32B32A32_UINT;
			case D3D_REGISTER_COMPONENT_SINT32:
				return DXGI_FORMAT_R32G32B32A32_SINT;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
			default:
				return DXGI_FORMAT_UNKNOWN;
			}
		}

		return DXGI_FORMAT_UNKNOWN;
	}

	UINT GetFormatSize(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R32_FLOAT:
			return 4;
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
		case DXGI_FORMAT_R32G32_FLOAT:
			return 8;
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
		case DXGI_FORMAT_R32G32B32_FLOAT:
			return 12;
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return 16;
		default:
			return 0;
		}
	}

	void BuildInputLayoutInfo(ID3D11ShaderReflection* reflection, ShaderInputLayoutInfo& info)
	{
		D3D11_SHADER_DESC shaderDesc{};
		reflection->GetDesc(&shaderDesc);
		UINT offset = 0;

		for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc{};
			reflection->GetInputParameterDesc(i, &paramDesc);
			if (paramDesc.SystemValueType != D3D_NAME_UNDEFINED)
			{
				continue;
			}

			auto format = GetInputFormat(paramDesc);
			if (format == DXGI_FORMAT_UNKNOWN)
			{
				continue;
			}

			InputElementInfo element{};
			if (paramDesc.SemanticName)
			{
				element.name = paramDesc.SemanticName;
			}
			element.index = paramDesc.SemanticIndex;
			element.format = format;
			element.offset = offset;
			info.elements.push_back(element);
			offset += GetFormatSize(format);
		}
	}

	void MergeShaderReflectionInfo(const ShaderReflectionInfo& source, ShaderReflectionInfo& target)
	{
		for (const auto& [name, info] : source.constantBuffers)
		{
			if (!target.constantBuffers.contains(name))
			{
				target.constantBuffers.emplace(name, info);
			}
		}

		for (const auto& [bufferName, bufferInfo] : source.buffers)
		{
			auto& targetBuffer = target.buffers[bufferName];
			targetBuffer.size = std::max(targetBuffer.size, bufferInfo.size);
			for (const auto& [varName, varInfo] : bufferInfo.variables)
			{
				if (!targetBuffer.variables.contains(varName))
				{
					targetBuffer.variables.emplace(varName, varInfo);
				}
			}
		}
	}
}

bool VertexShader::CreateBuffer(GraphicsDevice& device, const std::string& filePath)
{
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	if (!LoadShaderBlob(filePath, "vs_5_0", shaderBlob))
	{
		return false;
	}

	HRESULT hr = device.GetDevice()->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr,
		m_vs.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	m_shaderBlob = shaderBlob;

	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	if (!CreateShaderReflection(shaderBlob.Get(), reflection))
	{
		return false;
	}

	reflectionInfo = {};
	inputLayoutInfo = {};
	BuildShaderReflectionInfo(reflection.Get(), reflectionInfo);
	BuildInputLayoutInfo(reflection.Get(), inputLayoutInfo);
	return true;
}

void VertexShader::Bind(GraphicsDevice& device)
{
	device.GetDeviceContext()->VSSetShader(m_vs.Get(), nullptr, 0);
}

bool PixelShader::CreateBuffer(GraphicsDevice& device, const std::string& filePath)
{
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	if (!LoadShaderBlob(filePath, "ps_5_0", shaderBlob))
	{
		return false;
	}

	HRESULT hr = device.GetDevice()->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr,
		m_ps.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	if (!CreateShaderReflection(shaderBlob.Get(), reflection))
	{
		return false;
	}

	reflectionInfo = {};
	BuildShaderReflectionInfo(reflection.Get(), reflectionInfo);
	return true;
}

void PixelShader::Bind(GraphicsDevice& device)
{
	device.GetDeviceContext()->PSSetShader(m_ps.Get(), nullptr, 0);
}

bool ShaderProgram::CreateBuffer(GraphicsDevice& device, const std::string& vsFilePath, const std::string& psFilePath)
{
	if (!m_resourceSystem)
	{
		return false;
	}

	vertexShader = m_resourceSystem->Load<VertexShader>(vsFilePath);
	pixelShader = m_resourceSystem->Load<PixelShader>(psFilePath);
	if (!vertexShader || !pixelShader)
	{
		return false;
	}

	mergedReflectionInfo = {};
	MergeShaderReflectionInfo(vertexShader->reflectionInfo, mergedReflectionInfo);
	MergeShaderReflectionInfo(pixelShader->reflectionInfo, mergedReflectionInfo);
	inputLayoutInfo = vertexShader->inputLayoutInfo;

	m_inputLayout.Reset();
	auto* shaderBlob = vertexShader->GetShaderBlob();
	if (!shaderBlob || inputLayoutInfo.elements.empty())
	{
		return true;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	inputElements.reserve(inputLayoutInfo.elements.size());
	std::vector<std::string> semanticNames;
	semanticNames.reserve(inputLayoutInfo.elements.size());

	for (const auto& element : inputLayoutInfo.elements)
	{
		semanticNames.push_back(element.name);
		D3D11_INPUT_ELEMENT_DESC desc{};
		desc.SemanticName = semanticNames.back().c_str();
		desc.SemanticIndex = element.index;
		desc.Format = element.format;
		desc.InputSlot = 0;
		desc.AlignedByteOffset = element.offset;
		desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;
		inputElements.push_back(desc);
	}

	HRESULT hr = device.GetDevice()->CreateInputLayout(
		inputElements.data(),
		static_cast<UINT>(inputElements.size()),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		m_inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

void ShaderProgram::Bind(GraphicsDevice& device)
{
	if (m_inputLayout)
	{
		device.GetDeviceContext()->IASetInputLayout(m_inputLayout.Get());
	}
	if (vertexShader)
	{
		vertexShader->Bind(device);
	}
	if (pixelShader)
	{
		pixelShader->Bind(device);
	}
}
