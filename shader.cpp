/*==============================================================================

   シェーダー [shader.cpp]
														 Author : Youhei Sato
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "DebugOstream.h"
#include "GraphicsDevice.h"
#include <fstream>
#include <vector>
#include "Shader.h"
#include "ResourceSystem.h"

static ID3D11VertexShader* g_pVertexShader = nullptr;
static ID3D11InputLayout* g_pInputLayout = nullptr;

static ID3D11Buffer* g_pVSConstantBuffer0 = nullptr;// proj
static ID3D11Buffer* g_pVSConstantBuffer1 = nullptr;// world

static ID3D11PixelShader* g_pPixelShader = nullptr;
static ID3D11Buffer* g_pPSConstantBuffer = nullptr;


// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

namespace
{
	bool ReadCompiledShader(const char* path, std::vector<unsigned char>& data)
	{
		std::ifstream stream(path, std::ios::binary);
		if (!stream) {
			return false;
		}
		stream.seekg(0, std::ios::end);
		std::streamsize size = stream.tellg();
		stream.seekg(0, std::ios::beg);
		data.resize(static_cast<size_t>(size));
		if (!stream.read(reinterpret_cast<char*>(data.data()), size)) {
			return false;
		}
		return true;
	}

	class SpriteMaterialCB : public ConstantBuffer
	{
	public:
		struct CBData
		{
			DirectX::XMFLOAT4 Color = {};
		};
		SpriteMaterialCB()
			: ConstantBuffer(UsageType::DEFAULT)
		{
		}
		void CreateBuffers(GraphicsDevice& device) override
		{
			ConstantBuffer::CreateBuffers(&device, m_usageType, sizeof(CBData));
		}
		void UpdateBuffer(GraphicsDevice& device, const void* data) override
		{
			ConstantBuffer::UpdateBuffer(&device, m_usageType, data, sizeof(CBData));
			ID3D11Buffer* buffer = m_constantBuffer.Get();
			device.GetDeviceContext()->PSSetConstantBuffers(0, 1, &buffer);
		}
	};

	class MeshMaterialCB : public ConstantBuffer
	{
	public:
		struct CBData
		{
			DirectX::XMFLOAT4 Color = {};
		};
		MeshMaterialCB()
			: ConstantBuffer(UsageType::DEFAULT)
		{
		}
		void CreateBuffers(GraphicsDevice& device) override
		{
			ConstantBuffer::CreateBuffers(&device, m_usageType, sizeof(CBData));
		}
		void UpdateBuffer(GraphicsDevice& device, const void* data) override
		{
			ConstantBuffer::UpdateBuffer(&device, m_usageType, data, sizeof(CBData));
			ID3D11Buffer* buffer = m_constantBuffer.Get();
			device.GetDeviceContext()->PSSetConstantBuffers(1, 1, &buffer);
		}
	};
}


bool Shader_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr; // 戻り値格納用

	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		hal::dout << "Shader_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
		return false;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;


	// 事前コンパイル済み頂点シェーダーの読み込み
	std::ifstream ifs_vs("VertexShader2d.cso", std::ios::binary);

	if (!ifs_vs) {
		MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました\n\nshader_vertex_2d.cso", "エラー", MB_OK);
		return false;
	}

	// ファイルサイズを取得
	ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
	std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
	ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

	// バイナリデータを格納するためのバッファを確保
	unsigned char* vsbinary_pointer = new unsigned char[filesize];

	ifs_vs.read((char*)vsbinary_pointer, filesize); // バイナリデータを読み込む
	ifs_vs.close(); // ファイルを閉じる

	// 頂点シェーダーの作成
	hr = g_pDevice->CreateVertexShader(vsbinary_pointer, filesize, nullptr, &g_pVertexShader);

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : 頂点シェーダーの作成に失敗しました" << std::endl;
		delete[] vsbinary_pointer; // メモリリークしないようにバイナリデータのバッファを解放
		return false;
	}

	// 頂点レイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"    , 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得

	// 頂点レイアウトの作成
	hr = g_pDevice->CreateInputLayout(layout, num_elements, vsbinary_pointer, filesize, &g_pInputLayout);

	delete[] vsbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : 頂点レイアウトの作成に失敗しました" << std::endl;
		return false;
	}


	// 頂点シェーダー用定数バッファの作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(XMFLOAT4X4); // バッファのサイズ
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer0);
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer1);


	// 事前コンパイル済みピクセルシェーダーの読み込み
	std::ifstream ifs_ps("PixelShader2d.cso", std::ios::binary);
	if (!ifs_ps) {
		MessageBox(nullptr, "ピクセルシェーダーの読み込みに失敗しました\n\nshader_pixel_2d.cso", "エラー", MB_OK);
		return false;
	}

	ifs_ps.seekg(0, std::ios::end);
	filesize = ifs_ps.tellg();
	ifs_ps.seekg(0, std::ios::beg);

	unsigned char* psbinary_pointer = new unsigned char[filesize];
	ifs_ps.read((char*)psbinary_pointer, filesize);
	ifs_ps.close();

	// ピクセルシェーダーの作成
	hr = g_pDevice->CreatePixelShader(psbinary_pointer, filesize, nullptr, &g_pPixelShader);

	delete[] psbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : ピクセルシェーダーの作成に失敗しました" << std::endl;
		return false;
	}

	// ピクセルシェーダー用定数バッファの作成
	// D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(XMFLOAT4); // バッファのサイズ
	// buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer);


	return true;
}

void Shader_Finalize()
{
	SAFE_RELEASE(g_pPSConstantBuffer);
	SAFE_RELEASE(g_pPixelShader);

	SAFE_RELEASE(g_pVSConstantBuffer0);
	SAFE_RELEASE(g_pVSConstantBuffer1);

	SAFE_RELEASE(g_pInputLayout);
	SAFE_RELEASE(g_pVertexShader);
}

void Shader_SetColor(const DirectX::XMFLOAT4& color)
{
	// 定数バッファに色をセット
	g_pContext->UpdateSubresource(g_pPSConstantBuffer, 0, nullptr, &color, 0, 0);
}


void Shader_SetProjectionMatrix(const DirectX::XMMATRIX& matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列を転置して定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	g_pContext->UpdateSubresource(g_pVSConstantBuffer0, 0, nullptr, &transpose, 0, 0);
}

void Shader_SetWorldMatrix(const DirectX::XMMATRIX& matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列を転置して定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	g_pContext->UpdateSubresource(g_pVSConstantBuffer1, 0, nullptr, &transpose, 0, 0);
}


void Shader_Begin()
{
	// 頂点シェーダーとピクセルシェーダーを描画パイプラインに設定
	g_pContext->VSSetShader(g_pVertexShader, nullptr, 0);
	g_pContext->PSSetShader(g_pPixelShader, nullptr, 0);

	// 頂点レイアウトを描画パイプラインに設定
	g_pContext->IASetInputLayout(g_pInputLayout);

	// 定数バッファを描画パイプラインに設定	//b0,b1のスロットに送れる
	g_pContext->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer0);
	g_pContext->VSSetConstantBuffers(1, 1, &g_pVSConstantBuffer1);
	g_pContext->PSSetConstantBuffers(0, 1, &g_pPSConstantBuffer);

	// サンプラーステートをピクセルシェーダーに設定
	ID3D11SamplerState* samplerState = GetGraphicsDevice().GetSamplerState();
	g_pContext->PSSetSamplers(0, 1, &samplerState);

}

void SpriteVS::CreateBuffers(GraphicsDevice& device)
{
	if (!m_resourceSystem)
	{
		return;
	}
	std::vector<unsigned char> data;
	if (!ReadCompiledShader("SpriteVS.cso", data))
	{
		MessageBox(nullptr, "SpriteVSの読み込みに失敗しました", "エラー", MB_OK);
		return;
	}

	HRESULT hr = device.GetDevice()->CreateVertexShader(data.data(), data.size(), nullptr, m_vertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "SpriteVSの作成に失敗しました", "エラー", MB_OK);
		return;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"    , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device.GetDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), data.data(), data.size(), m_inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "SpriteVSの入力レイアウト作成に失敗しました", "エラー", MB_OK);
		return;
	}

	m_perCameraCB = m_resourceSystem->Load<PerCameraCB>();
	m_perObjectCB = m_resourceSystem->Load<PerObjectCB>();
}

void SpriteVS::Bind(GraphicsDevice& device)
{
	auto* context = device.GetDeviceContext();
	context->IASetInputLayout(m_inputLayout.Get());
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	if (m_perCameraCB)
	{
		ID3D11Buffer* buffer = m_perCameraCB->GetBuffer();
		context->VSSetConstantBuffers(0, 1, &buffer);
	}
	if (m_perObjectCB)
	{
		ID3D11Buffer* buffer = m_perObjectCB->GetBuffer();
		context->VSSetConstantBuffers(1, 1, &buffer);
	}
}

void SpritePS::CreateBuffers(GraphicsDevice& device)
{
	if (!m_resourceSystem)
	{
		return;
	}
	std::vector<unsigned char> data;
	if (!ReadCompiledShader("SpritePS.cso", data))
	{
		MessageBox(nullptr, "SpritePSの読み込みに失敗しました", "エラー", MB_OK);
		return;
	}

	HRESULT hr = device.GetDevice()->CreatePixelShader(data.data(), data.size(), nullptr, m_pixelShader.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "SpritePSの作成に失敗しました", "エラー", MB_OK);
		return;
	}

	m_materialCB = m_resourceSystem->Load<SpriteMaterialCB>();
}

void SpritePS::Bind(GraphicsDevice& device)
{
	auto* context = device.GetDeviceContext();
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	if (m_materialCB)
	{
		ID3D11Buffer* buffer = m_materialCB->GetBuffer();
		context->PSSetConstantBuffers(0, 1, &buffer);
	}
	ID3D11SamplerState* samplerState = device.GetSamplerState();
	context->PSSetSamplers(0, 1, &samplerState);
}

void MeshVS::CreateBuffers(GraphicsDevice& device)
{
	if (!m_resourceSystem)
	{
		return;
	}
	std::vector<unsigned char> data;
	if (!ReadCompiledShader("MeshVS.cso", data))
	{
		MessageBox(nullptr, "MeshVSの読み込みに失敗しました", "エラー", MB_OK);
		return;
	}

	HRESULT hr = device.GetDevice()->CreateVertexShader(data.data(), data.size(), nullptr, m_vertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "MeshVSの作成に失敗しました", "エラー", MB_OK);
		return;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"    , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL"   , 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device.GetDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), data.data(), data.size(), m_inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "MeshVSの入力レイアウト作成に失敗しました", "エラー", MB_OK);
		return;
	}

	m_perCameraCB = m_resourceSystem->Load<PerCameraCB>();
	m_perObjectCB = m_resourceSystem->Load<PerObjectCB>();
}

void MeshVS::Bind(GraphicsDevice& device)
{
	auto* context = device.GetDeviceContext();
	context->IASetInputLayout(m_inputLayout.Get());
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	if (m_perObjectCB)
	{
		ID3D11Buffer* buffer = m_perObjectCB->GetBuffer();
		context->VSSetConstantBuffers(0, 1, &buffer);
	}
	if (m_perCameraCB)
	{
		ID3D11Buffer* buffer = m_perCameraCB->GetBuffer();
		context->VSSetConstantBuffers(1, 1, &buffer);
	}
}

void MeshPS::CreateBuffers(GraphicsDevice& device)
{
	if (!m_resourceSystem)
	{
		return;
	}
	std::vector<unsigned char> data;
	if (!ReadCompiledShader("MeshPS.cso", data))
	{
		MessageBox(nullptr, "MeshPSの読み込みに失敗しました", "エラー", MB_OK);
		return;
	}

	HRESULT hr = device.GetDevice()->CreatePixelShader(data.data(), data.size(), nullptr, m_pixelShader.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, "MeshPSの作成に失敗しました", "エラー", MB_OK);
		return;
	}

	m_perFrameCB = m_resourceSystem->Load<PerFrameCB>();
	m_materialCB = m_resourceSystem->Load<MeshMaterialCB>();
}

void MeshPS::Bind(GraphicsDevice& device)
{
	auto* context = device.GetDeviceContext();
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	if (m_perFrameCB)
	{
		ID3D11Buffer* buffer = m_perFrameCB->GetBuffer();
		context->PSSetConstantBuffers(0, 1, &buffer);
	}
	if (m_materialCB)
	{
		ID3D11Buffer* buffer = m_materialCB->GetBuffer();
		context->PSSetConstantBuffers(1, 1, &buffer);
	}
	ID3D11SamplerState* samplerState = device.GetSamplerState();
	context->PSSetSamplers(0, 1, &samplerState);
}

