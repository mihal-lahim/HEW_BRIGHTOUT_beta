/*==============================================================================

   シェーダー [shader.cpp]
														 Author : Youhei Sato
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "DebugOstream.h"
#include "GraphicsDevice.h"
#include <fstream>
#include <vector>
#include "Shader.h"
#include "ResourceSystem.h"



// コンパイル済みシェーダーファイルを読み込む
bool Shader::ReadCompiledShader(const char* path, Microsoft::WRL::ComPtr<ID3DBlob>& blob)
{
	// ファイルをバイナリモードで開く
	std::ifstream stream(path, std::ios::binary);
	if (!stream) {
		return false;
	}

	// ファイルサイズを取得してデータを読み込む
	stream.seekg(0, std::ios::end);
	std::streamsize size = stream.tellg();
	stream.seekg(0, std::ios::beg);
	std::vector<unsigned char> data(static_cast<size_t>(size));
	if (!stream.read(reinterpret_cast<char*>(data.data()), size)) {
		return false;
	}

	// ID3DBlobにデータをコピーする
	Microsoft::WRL::ComPtr<ID3DBlob> buffer;
	if (FAILED(D3DCreateBlob(data.size(), buffer.GetAddressOf()))) {
		return false;
	}
	memcpy(buffer->GetBufferPointer(), data.data(), data.size());
	blob = buffer;
	return true;
}

// シェーダーの作成
bool Shader::CreateShaderFromFile(GraphicsDevice& device, const char* path, Microsoft::WRL::ComPtr<ID3DBlob>* shaderBlob)
{
	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	if (!ReadCompiledShader(path, blob)) {
		return false;
	}

	HRESULT hr = E_FAIL;
	switch (m_type) {
	case ShaderType::Vertex:
		hr = device.GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
		break;
	case ShaderType::Pixel:
		hr = device.GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
		break;
	default:
		return false;
	}

	if (FAILED(hr)) {
		return false;
	}

	if (shaderBlob) {
		*shaderBlob = blob;
	}
	return true;
}

// 頂点入力要素のフォーマットを取得する
DXGI_FORMAT VertexShader::GetInputElementFormat(const D3D11_SIGNATURE_PARAMETER_DESC& desc)
{
	if (desc.Mask == 1) {
		switch (desc.ComponentType) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32_FLOAT;
		}
	} else if (desc.Mask <= 3) {
		switch (desc.ComponentType) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32_FLOAT;
		}
	} else if (desc.Mask <= 7) {
		switch (desc.ComponentType) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32B32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32B32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		}
	} else if (desc.Mask <= 15) {
		switch (desc.ComponentType) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32B32A32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	}

	return DXGI_FORMAT_UNKNOWN;
}

// シェーダーブロブから入力レイアウトを作成する
bool VertexShader::CreateInputLayoutFromBlob(GraphicsDevice& device, ID3DBlob* blob, Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout)
{
	if (!blob) {
		return false;
	}

	// シェーダーリフレクションの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
	if (FAILED(D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(reflection.GetAddressOf())))) {
		return false;
	}

	// シェーダーの説明を取得
	D3D11_SHADER_DESC shaderDesc{};
	if (FAILED(reflection->GetDesc(&shaderDesc))) {
		return false;
	}

	// 入力要素の説明を作成
	std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDescs;
	layoutDescs.reserve(shaderDesc.InputParameters);

	// 各入力パラメータについて処理
	for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc{};
		if (FAILED(reflection->GetInputParameterDesc(i, &paramDesc))) {
			return false;
		}

		// 入力要素のフォーマットを取得
		DXGI_FORMAT format = GetInputElementFormat(paramDesc);
		if (format == DXGI_FORMAT_UNKNOWN) {
			return false;
		}

		// 入力要素の説明を設定
		D3D11_INPUT_ELEMENT_DESC elementDesc{};
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.Format = format;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;
		layoutDescs.push_back(elementDesc);
	}

	// 入力レイアウトの作成
	return SUCCEEDED(device.GetDevice()->CreateInputLayout(
		layoutDescs.data(),
		static_cast<UINT>(layoutDescs.size()),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		inputLayout.GetAddressOf()));
}

// 頂点シェーダーと入力レイアウトの作成
bool VertexShader::CreateVertexShaderWithLayout(GraphicsDevice& device, const char* path)
{
	// シェーダーの作成
	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	if (!CreateShaderFromFile(device, path, &blob)) {
		return false;
	}

	// 入力レイアウトの作成
	return CreateInputLayoutFromBlob(device, blob.Get(), m_inputLayout);
}



bool SpriteVS::CreateBuffer(GraphicsDevice& device)
{
	if (!m_resourceSystem)
	{
		return false;
	}

	if (!CreateVertexShaderWithLayout(device, "SpriteVS.cso"))
	{
		return false;
	}

	return true;
}

void SpriteVS::Bind(GraphicsDevice& device)
{
	auto* context = device.GetDeviceContext();
	context->IASetInputLayout(m_inputLayout.Get());
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
}

bool SpritePS::CreateBuffer(GraphicsDevice& device)
{
	if (!m_resourceSystem)
	{
		return false;
	}
	if (!CreateShaderFromFile(device, "SpritePS.cso"))
	{
		return false;
	}

	return true;
}

void SpritePS::Bind(GraphicsDevice& device)
{
	auto* context = device.GetDeviceContext();
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	ID3D11SamplerState* samplerState = device.GetSamplerState();
	context->PSSetSamplers(0, 1, &samplerState);
}

bool MeshVS::CreateBuffer(GraphicsDevice& device)
{
	if (!m_resourceSystem)
	{
		return false;
	}
	if (!CreateVertexShaderWithLayout(device, "MeshVS.cso"))
	{
		return false;
	}

	return true;
}

void MeshVS::Bind(GraphicsDevice& device)
{
	auto* context = device.GetDeviceContext();
	context->IASetInputLayout(m_inputLayout.Get());
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
}

bool MeshPS::CreateBuffer(GraphicsDevice& device)
{
	if (!m_resourceSystem)
	{
		return false;
	}
	if (!CreateShaderFromFile(device, "MeshPS.cso"))
	{
		return false;
	}

	return true;
}

void MeshPS::Bind(GraphicsDevice& device)
{
	auto* context = device.GetDeviceContext();
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	ID3D11SamplerState* samplerState = device.GetSamplerState();
	context->PSSetSamplers(0, 1, &samplerState);
}
