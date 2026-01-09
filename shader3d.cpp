/*==============================================================================

   3D用シェーダー [shader3d.cpp]
														 Author : Rintarou Sumi
														 Date   : 2025/10/16
--------------------------------------------------------------------------------

==============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"
#include "debug_ostream.h"
#include <fstream>
#include "shader3d.h"

static ID3D11VertexShader* g_pVertexShader = nullptr;// 頂点シェーダー
static ID3D11InputLayout* g_pInputLayout = nullptr;// 頂点レイアウト
static ID3D11Buffer* g_pVSConstantBuffer0 = nullptr;
static ID3D11Buffer* g_pVSConstantBuffer1 = nullptr;
static ID3D11Buffer* g_pVSConstantBuffer2 = nullptr;
static ID3D11Buffer* g_pPSConstantBuffer2 = nullptr;
static ID3D11Buffer* g_pPSMaterialCB = nullptr;  // オーバーライド用マテリアルCB
static ID3D11PixelShader* g_pPixelShader = nullptr;
static ID3D11SamplerState* g_pSamplerState = nullptr;

// 注意！テクスチャは外側で設定される。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;


bool Shader3d_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr; // 戻り値格納用

	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		hal::dout << "Shader_Initialize() : 不正なデバイスまたはコンテキストです" << std::endl;
		return false;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;


	// ログコンパイル済み頂点シェーダーの読み込み
	std::ifstream ifs_vs("VertexShader3d.cso", std::ios::binary);

	if (!ifs_vs) {
		MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました\n\nshader_vertex_3d.cso", "エラー", MB_OK);
		return false;
	}

	// ファイルサイズを取得
	ifs_vs.seekg(0, std::ios::end); // ファイルポインタを終端に移動
	std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
	ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

	// バイナルデータを格納するためのバッファを確保
	unsigned char* vsbinary_pointer = new unsigned char[filesize];
	
	ifs_vs.read((char*)vsbinary_pointer, filesize); // バイナリデータを読み込み
	ifs_vs.close(); // ファイルを閉じる

	// 頂点シェーダーの作成
	hr = g_pDevice->CreateVertexShader(vsbinary_pointer, filesize, nullptr, &g_pVertexShader);

	if (FAILED(hr)) {
		hal::dout << "Shader3d_Initialize() : 頂点シェーダーの作成に失敗しました" << std::endl;
		delete[] vsbinary_pointer; // メモリリークを起こさないようにバイナリデータのバッファを削除
		return false;
	}

	// 頂点レイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL"   , 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"    , 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES" , 0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得

	// 頂点レイアウトの作成
	hr = g_pDevice->CreateInputLayout(layout, num_elements, vsbinary_pointer, filesize, &g_pInputLayout);

	delete[] vsbinary_pointer; // バイナリデータのバッファを削除

	if (FAILED(hr)) {
		hal::dout << "Shader3d_Initialize() : 頂点レイアウトの作成に失敗しました" << std::endl;
		return false;
	}

	// 頂点シェーダー用定数バッファの作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(XMFLOAT4X4); // バッファのサイズ
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer0);
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer1);
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer2);

	// ピクセルシェーダー用定数バッファの作成
	buffer_desc.ByteWidth = sizeof(XMFLOAT4); // バッファのサイズ
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer2);

	// マテリアルオーバーライド定数バッファ
	buffer_desc.ByteWidth = sizeof(MaterialCB);
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSMaterialCB);

	// ログコンパイル済みピクセルシェーダーの読み込み
	std::ifstream ifs_ps("PixelShader3d.cso", std::ios::binary);
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

	delete[] psbinary_pointer; // バイナリデータのバッファを削除

	if (FAILED(hr)) {
		hal::dout << "Shader3d_Initialize() : ピクセルシェーダーの作成に失敗しました" << std::endl;
		return false;
	}

	// ピクセルシェーダー用定数バッファの作成
	buffer_desc.ByteWidth = sizeof(XMFLOAT4); // バッファのサイズ
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer2);

	// サンプラーステート設定
	D3D11_SAMPLER_DESC sampler_desc{};
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;// 3Dでのテクスチャは遠い角度のほうが旨い(ソフト重視)
	//sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;//ドット
	//sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//sampler_desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;

	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	////ボーダーを使う場合 D3D11_TEXTURE_ADDRESS_BORDER
	sampler_desc.BorderColor[0] = 1.0f;
	sampler_desc.BorderColor[1] = 1.0f;
	sampler_desc.BorderColor[2] = 0.0f;
	sampler_desc.BorderColor[3] = 0.0f;

	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 8;// 最大のピクセル幅は16程度耐えられる(2,4,8,16)
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerState);

	return true;
}

void Shader3d_Finalize()
{
	SAFE_RELEASE(g_pSamplerState);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pVSConstantBuffer0);
	SAFE_RELEASE(g_pVSConstantBuffer1);
	SAFE_RELEASE(g_pVSConstantBuffer2);
	SAFE_RELEASE(g_pPSConstantBuffer2);
	SAFE_RELEASE(g_pPSMaterialCB);
	SAFE_RELEASE(g_pInputLayout);
	SAFE_RELEASE(g_pVertexShader);
}

void Shader3d_SetWorldMatrix(const DirectX::XMMATRIX& matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列転置してから定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	g_pContext->UpdateSubresource(g_pVSConstantBuffer0, 0, nullptr, &transpose, 0, 0);
}

void Shader3d_SetViewMatrix(const DirectX::XMMATRIX& matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列転置してから定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	g_pContext->UpdateSubresource(g_pVSConstantBuffer1, 0, nullptr, &transpose, 0, 0);
}

void Shader3d_SetProjectionMatrix(const DirectX::XMMATRIX& matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列転置してから定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	g_pContext->UpdateSubresource(g_pVSConstantBuffer2, 0, nullptr, &transpose, 0, 0);
}

void Shader3d_SetMaterialDiffuse(const DirectX::XMFLOAT4 color)
{
	// 定数バッファに行列をセット
	g_pContext->UpdateSubresource(g_pPSConstantBuffer2, 0, nullptr, &color, 0, 0);
}

void Shader3d_SetMaterialCB(const MaterialCB& mtlCB)
{
	// マテリアルオーバーライド定数バッファ更新
	g_pContext->UpdateSubresource(g_pPSMaterialCB, 0, nullptr, &mtlCB, 0, 0);
}

void Shader3d_Begin()
{
	// 頂点シェーダーとピクセルシェーダーを描画パイプラインに設定
	g_pContext->VSSetShader(g_pVertexShader, nullptr, 0);
	g_pContext->PSSetShader(g_pPixelShader, nullptr, 0);

	// 頂点レイアウトを描画パイプラインに設定
	g_pContext->IASetInputLayout(g_pInputLayout);

	// 定数バッファを描画パイプラインに設定	//b0,b1のスロットに関連
	g_pContext->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer0);
	g_pContext->VSSetConstantBuffers(1, 1, &g_pVSConstantBuffer1);
	g_pContext->VSSetConstantBuffers(2, 1, &g_pVSConstantBuffer2);
	g_pContext->PSSetConstantBuffers(2, 1, &g_pPSConstantBuffer2);
	g_pContext->PSSetConstantBuffers(3, 1, &g_pPSMaterialCB);

	// サンプラーステートをピクセルシェーダーに設定
	g_pContext->PSSetSamplers(0, 1, &g_pSamplerState);
}
