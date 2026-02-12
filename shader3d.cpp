///*==============================================================================
//
//   シェーダー [shader3d.cpp]
//														 Author : Youhei Sato
//														 Date   : 2025/10/15
//--------------------------------------------------------------------------------
//
//==============================================================================*/
//#include <d3d11.h>
//#include <DirectXMath.h>
//using namespace DirectX;
//#include "GraphicsDevice.h"
//#include <iostream>
//#include <fstream>
//
//// 追加：ラスタライザステートを保持
//static ID3D11VertexShader* g_pVertexShader = nullptr;
//static ID3D11InputLayout* g_pInputLayout = nullptr;
//
//static ID3D11Buffer* g_pVSConstantBuffer0 = nullptr;
//static ID3D11Buffer* g_pVSConstantBuffer1 = nullptr;
//static ID3D11Buffer* g_pVSConstantBuffer2 = nullptr;
//
//static ID3D11Buffer* g_pPSConstantBuffer2 = nullptr;
//
//static ID3D11PixelShader* g_pPixelShader = nullptr;
//static ID3D11SamplerState* g_pSamplerState = nullptr;
//
//static ID3D11RasterizerState* g_pRasterizerNoCull = nullptr; // 追加
//
//// 注意！初期化で外部から設定されるもの。Release不要。
//static ID3D11Device* g_pDevice = nullptr;
//static ID3D11DeviceContext* g_pContext = nullptr;
//
//
//bool Shader3d_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//{
//	HRESULT hr; // 戻り値格納用
//
//	// デバイスとデバイスコンテキストのチェック
//	if (!pDevice || !pContext) {
//		//hal::dout << "Shader_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
//		return false;
//	}
//
//	// デバイスとデバイスコンテキストの保存
//	g_pDevice = pDevice;
//	g_pContext = pContext;
//
//
//	// 事前コンパイル済み頂点シェーダーの読み込み
//	std::ifstream ifs_vs("VertexShader3d.cso", std::ios::binary);
//
//	if (!ifs_vs) {
//		MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました\n\nshader_vertex_2d.cso", "エラー", MB_OK);
//		return false;
//	}
//
//	// ファイルサイズを取得
//	ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
//	std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
//	ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す
//
//	// バイナリデータを格納するためのバッファを確保
//	unsigned char* vsbinary_pointer = new unsigned char[filesize];
//
//	ifs_vs.read((char*)vsbinary_pointer, filesize); // バイナリデータを読み込む
//	ifs_vs.close(); // ファイルを閉じる
//
//	// 頂点シェーダーの作成
//	hr = g_pDevice->CreateVertexShader(vsbinary_pointer, filesize, nullptr, &g_pVertexShader);
//
//	if (FAILED(hr)) {
//		//hal::dout << "Shader_Initialize() : 頂点シェーダーの作成に失敗しました" << std::endl;
//		delete[] vsbinary_pointer; // メモリリークしないようにバイナリデータのバッファを解放
//		return false;
//	}
//
//
//	// 頂点レイアウトの定義
//	D3D11_INPUT_ELEMENT_DESC layout[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//
//	UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得
//
//	// 頂点レイアウトの作成
//	hr = g_pDevice->CreateInputLayout(layout, num_elements, vsbinary_pointer, filesize, &g_pInputLayout);
//
//	delete[] vsbinary_pointer; // バイナリデータのバッファを解放
//
//	if (FAILED(hr)) {
//		//hal::dout << "Shader_Initialize() : 頂点レイアウトの作成に失敗しました" << std::endl;
//		return false;
//	}
//
//
//	// 頂点シェーダー用定数バッファの作成
//	D3D11_BUFFER_DESC buffer_desc{};
//	buffer_desc.ByteWidth = sizeof(XMFLOAT4X4); // バッファのサイズ
//	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ
//
//	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer0);
//
//
//	// 事前コンパイル済みピクセルシェーダーの読み込み
//	std::ifstream ifs_ps("PixelShader3d.cso", std::ios::binary);
//
//	if (!ifs_ps) {
//		MessageBox(nullptr, "ピクセルシェーダーの読み込みに失敗しました\n\nshader_pixel_3d.cso", "エラー", MB_OK);
//		return false;
//	}
//
//	ifs_ps.seekg(0, std::ios::end);
//	filesize = ifs_ps.tellg();
//	ifs_ps.seekg(0, std::ios::beg);
//
//	unsigned char* psbinary_pointer = new unsigned char[filesize];
//	ifs_ps.read((char*)psbinary_pointer, filesize);
//	ifs_ps.close();
//
//	// ピクセルシェーダーの作成
//	hr = g_pDevice->CreatePixelShader(psbinary_pointer, filesize, nullptr, &g_pPixelShader);
//
//	delete[] psbinary_pointer; // バイナリデータのバッファを解放
//
//	//ピクセルシェーダー用定数バッファ
//	buffer_desc.ByteWidth = sizeof(XMFLOAT4); // バッファのサイズ
//	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer2);
//
//
//	// サンプラーステート設定
//	D3D11_SAMPLER_DESC sampler_desc{};
//	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;//POINT
//	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//MIRROR , BORDER
//	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;//MIRROR , BORDER
//	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
//	sampler_desc.BorderColor[0] = 1.0f;
//	sampler_desc.BorderColor[1] = 1.0f;
//	sampler_desc.BorderColor[2] = 1.0f;
//	sampler_desc.BorderColor[3] = 1.0f;
//	sampler_desc.MipLODBias = 0;
//	sampler_desc.MaxAnisotropy = 16;
//	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//	sampler_desc.MinLOD = 0;
//	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
//
//	g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerState);
//
//	return true;
//}
//
//void Shader3d_Finalize()
//{
//	SAFE_RELEASE(g_pSamplerState);
//	SAFE_RELEASE(g_pPixelShader);
//	SAFE_RELEASE(g_pPSConstantBuffer2);
//	SAFE_RELEASE(g_pVSConstantBuffer0);
//	SAFE_RELEASE(g_pInputLayout);
//	SAFE_RELEASE(g_pVertexShader);
//}
//
//
//void Shader3d_SetWorldMatrix(const DirectX::XMMATRIX& matrix)
//{
//	// 定数バッファ格納用行列の構造体を定義
//	XMFLOAT4X4 transpose;
//
//	// 行列を転置して定数バッファ格納用行列に変換
//	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));
//
//	// 定数バッファに行列をセット
//	g_pContext->UpdateSubresource(g_pVSConstantBuffer0, 0, nullptr, &transpose, 0, 0);
//}
//
//void Shader3d_SetViewMatrix(const DirectX::XMMATRIX& matrix)
//{
//	// 定数バッファ格納用行列の構造体を定義
//	XMFLOAT4X4 transpose;
//
//	// 行列を転置して定数バッファ格納用行列に変換
//	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));
//
//
//}
//
//void Shader3d_SetProjectionMatrix(const DirectX::XMMATRIX& matrix)
//{
//	// 定数バッファ格納用行列の構造体を定義
//	XMFLOAT4X4 transpose;
//
//	// 行列を転置して定数バッファ格納用行列に変換
//	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));
//
//}
//
//void Shader3d_SetMaterialDiffuse(const DirectX::XMFLOAT4 color)
//{
//	// 定数バッファに行列をセット
//	g_pContext->UpdateSubresource(g_pPSConstantBuffer2, 0, nullptr, &color, 0, 0);
//}
//
//void Shader3d_Begin()
//{
//	// 頂点シェーダーとピクセルシェーダーを描画パイプラインに設定
//	g_pContext->VSSetShader(g_pVertexShader, nullptr, 0);
//	g_pContext->PSSetShader(g_pPixelShader, nullptr, 0);
//
//	// 頂点レイアウトを描画パイプラインに設定
//	g_pContext->IASetInputLayout(g_pInputLayout);
//
//	// 定数バッファを描画パイプラインに設定
//	g_pContext->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer0);
//	g_pContext->PSSetConstantBuffers(2, 1, &g_pPSConstantBuffer2);
//
//
//	//サンプラーステートを描画パイプラインに設定
//	g_pContext->PSSetSamplers(0, 1, &g_pSamplerState);
//}
/*==============================================================================
   シェーダー [shader3d.cpp]
==============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <windows.h>
#include "shader3d.h"

using namespace DirectX;

//======================================================
// 定数バッファ構造体（HLSL と完全一致）
//======================================================

// MeshVS.hlsl
struct CB_PER_OBJECT
{
    XMFLOAT4X4 world;
};

struct CB_PER_CAMERA
{
    XMFLOAT4X4 view;
    XMFLOAT4X4 proj;
};

// MeshPS.hlsl
struct CB_PER_FRAME
{
    XMFLOAT4 ambient_light_color;
    XMFLOAT4 directional_light_color;
    XMFLOAT4 directional_light_vector;
};

struct CB_PER_MATERIAL
{
    XMFLOAT4 material_diffuse_color;
};

//======================================================
// グローバル
//======================================================
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static ID3D11VertexShader* g_pVS = nullptr;
static ID3D11PixelShader* g_pPS = nullptr;
static ID3D11InputLayout* g_pLayout = nullptr;
static ID3D11SamplerState* g_pSampler = nullptr;

// 定数バッファ
static ID3D11Buffer* g_cbPerObject = nullptr; // b0 (world)
static ID3D11Buffer* g_cbPerCamera = nullptr; // b1 (view, proj)
static ID3D11Buffer* g_cbPerMaterial = nullptr; // b1 (PS)
static ID3D11Buffer* g_cbPerFrame = nullptr; // b0 (PS)

static CB_PER_CAMERA g_cbCamera{};


//======================================================
// 初期化
//======================================================
bool Shader3d_Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
    g_pDevice = device;
    g_pContext = context;

    HRESULT hr;

    //==============================
    // Vertex Shader
    //==============================
    std::ifstream vsFile("MeshVS.cso", std::ios::binary);
    if (!vsFile) return false;

    vsFile.seekg(0, std::ios::end);
    size_t vsSize = vsFile.tellg();
    vsFile.seekg(0);

    char* vsData = new char[vsSize];
    vsFile.read(vsData, vsSize);
    vsFile.close();

    hr = g_pDevice->CreateVertexShader(vsData, vsSize, nullptr, &g_pVS);
    if (FAILED(hr)) return false;

    // InputLayout（Vertex3d と完全一致）
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = g_pDevice->CreateInputLayout(
        layout, _countof(layout),
        vsData, vsSize,
        &g_pLayout
    );

    delete[] vsData;
    if (FAILED(hr)) return false;

    //==============================
    // Pixel Shader
    //==============================
    std::ifstream psFile("MeshPS.cso", std::ios::binary);
    if (!psFile) return false;

    psFile.seekg(0, std::ios::end);
    size_t psSize = psFile.tellg();
    psFile.seekg(0);

    char* psData = new char[psSize];
    psFile.read(psData, psSize);
    psFile.close();

    hr = g_pDevice->CreatePixelShader(psData, psSize, nullptr, &g_pPS);
    delete[] psData;
    if (FAILED(hr)) return false;

    //==============================
    // 定数バッファ作成
    //==============================
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    bd.ByteWidth = sizeof(CB_PER_OBJECT);
    g_pDevice->CreateBuffer(&bd, nullptr, &g_cbPerObject);

    bd.ByteWidth = sizeof(CB_PER_CAMERA);
    g_pDevice->CreateBuffer(&bd, nullptr, &g_cbPerCamera);

    bd.ByteWidth = sizeof(CB_PER_FRAME);
    g_pDevice->CreateBuffer(&bd, nullptr, &g_cbPerFrame);

    bd.ByteWidth = sizeof(CB_PER_MATERIAL);
    g_pDevice->CreateBuffer(&bd, nullptr, &g_cbPerMaterial);

    //==============================
    // Sampler
    //==============================
    D3D11_SAMPLER_DESC sd{};
    sd.Filter = D3D11_FILTER_ANISOTROPIC;
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.MaxAnisotropy = 16;
    sd.MaxLOD = D3D11_FLOAT32_MAX;

    g_pDevice->CreateSamplerState(&sd, &g_pSampler);

    return true;
}

void Shader3d_Finalize()
{
}

//======================================================
// 行列セット
//======================================================
void Shader3d_SetWorldMatrix(const XMMATRIX& m)
{
    CB_PER_OBJECT cb{};
    XMStoreFloat4x4(&cb.world, XMMatrixTranspose(m));
    g_pContext->UpdateSubresource(g_cbPerObject, 0, nullptr, &cb, 0, 0);
}

void Shader3d_SetViewMatrix(const XMMATRIX& m)
{
    XMStoreFloat4x4(&g_cbCamera.view, XMMatrixTranspose(m));
    g_pContext->UpdateSubresource(g_cbPerCamera, 0, nullptr, &g_cbCamera, 0, 0);
}


void Shader3d_SetProjectionMatrix(const XMMATRIX& m)
{
    XMStoreFloat4x4(&g_cbCamera.proj, XMMatrixTranspose(m));
    g_pContext->UpdateSubresource(g_cbPerCamera, 0, nullptr, &g_cbCamera, 0, 0);
}

//======================================================
// ライト・マテリアル
//======================================================

void Shader3d_SetLight(
    const XMFLOAT4& ambient,
    const XMFLOAT4& dirColor,
    const XMFLOAT4& dirVec)
{
    CB_PER_FRAME cb{};
    cb.ambient_light_color = ambient;
    cb.directional_light_color = dirColor;
    cb.directional_light_vector = dirVec;
    g_pContext->UpdateSubresource(g_cbPerFrame, 0, nullptr, &cb, 0, 0);
}

void Shader3d_SetMaterialDiffuse(const XMFLOAT4& color)
{
    CB_PER_MATERIAL cb{};
    cb.material_diffuse_color = color;
    g_pContext->UpdateSubresource(g_cbPerMaterial, 0, nullptr, &cb, 0, 0);
}

//======================================================
// 描画開始
//======================================================
void Shader3d_Begin()
{
    g_pContext->IASetInputLayout(g_pLayout);
    g_pContext->VSSetShader(g_pVS, nullptr, 0);
    g_pContext->PSSetShader(g_pPS, nullptr, 0);

    // VS
    g_pContext->VSSetConstantBuffers(0, 1, &g_cbPerObject);
    g_pContext->VSSetConstantBuffers(1, 1, &g_cbPerCamera);

    // PS
    g_pContext->PSSetConstantBuffers(0, 1, &g_cbPerFrame);
    g_pContext->PSSetConstantBuffers(1, 1, &g_cbPerMaterial);

    g_pContext->PSSetSamplers(0, 1, &g_pSampler);
}

void Shader3d_SetCamera(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{
    Shader3d_SetViewMatrix(view);
    Shader3d_SetProjectionMatrix(proj);
}