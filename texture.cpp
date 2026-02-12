#include "texture.h"
#include <string>
#include "GraphicsDevice.h"
#include "DirectXTex.h"
#include <stdexcept>
#include <sstream>
#include <Windows.h>
using namespace DirectX;
static constexpr int TEXTURE_MAX =1024;//テクスチャの最大数

struct texture
{
	std::wstring filename;//実ファイルパス
	unsigned int width;
	unsigned int height;

	ID3D11ShaderResourceView* pTexture = nullptr;
};

static texture g_Textures[TEXTURE_MAX];

// デバイス参照
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

// デフォルトテクスチャID
static int g_DefaultTexture = -1;

// デバッグ出力ユーティリティ
static void DBG(const char* fmt, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf_s(buf, _countof(buf), _TRUNCATE, fmt, ap);
	va_end(ap);
	OutputDebugStringA(buf);
	OutputDebugStringA("\n");
}

void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	g_pDevice = pDevice;
	g_pContext = pContext;

	// デフォルトテクスチャ読み込み
	//DBG("Texture_Initialize: loading fallback white texture");
	g_DefaultTexture = Texture_Load(L"texture/white.png", false);
	//DBG("Texture_Initialize: default texture id=%d", g_DefaultTexture);
}

void Texture_Finalize()
{
	Texture_AllRelease();
}

int Texture_Load(const wchar_t* pFileName, bool bMipMap)
{
	DBG("Texture_Load: \"%S\" bMipMap=%d", pFileName ? pFileName : L"(null)", bMipMap);

	//既に読み込まれているかチェック
	for (int i =0; i < TEXTURE_MAX; i++)
	{
		if (!g_Textures[i].pTexture) continue;

		if (g_Textures[i].filename == pFileName)
		{
			DBG("Texture_Load: already loaded id=%d path=\"%S\"", i, pFileName);
			return i;
		}
	}

	// 空きスロットへ読み込み
	for (int i =0; i < TEXTURE_MAX; i++)
	{
		if (g_Textures[i].pTexture) continue;

		// テクスチャ読み込み
		TexMetadata metadata;
		ScratchImage image;

		HRESULT hr = LoadFromWICFile(pFileName, WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr)) {
			wchar_t curDir[MAX_PATH] = {};
			GetCurrentDirectoryW(MAX_PATH, curDir);
			std::wstringstream ss;
			ss << L"Texture load failed: \n" << pFileName << L"\n";
			ss << L"Working directory: \n" << curDir << L"\n";
			ss << L"HRESULT:0x" << std::hex << hr;
			std::wstring msg = ss.str();
			MessageBoxW(nullptr, msg.c_str(), L"Texture Load Error", MB_OK | MB_ICONERROR);
			OutputDebugStringW(msg.c_str());
			DBG("Texture_Load FAILED hr=0x%08X", hr);
			return -1;
		}

		g_Textures[i].width = (unsigned int)metadata.width;
		g_Textures[i].height = (unsigned int)metadata.height;

		if (bMipMap)
		{
			ScratchImage mipChain;
			GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), TEX_FILTER_DEFAULT,0, mipChain);

			image = std::move(mipChain);
			metadata = image.GetMetadata();
		}

		hr = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, &g_Textures[i].pTexture);

		if (FAILED(hr))
		{
			MessageBox(nullptr, "画像の読み込みに失敗しました", "エラー", MB_OK | MB_ICONERROR);
			DBG("CreateShaderResourceView FAILED hr=0x%08X", hr);
			// 続行せずに -1 を返す
			return -1;
		}

		// ファイル名を保存
		g_Textures[i].filename = pFileName;

		DBG("Texture_Load: success id=%d size=(%u,%u)", i, g_Textures[i].width, g_Textures[i].height);
		return i;
	}
	DBG("Texture_Load: no free slot");
	return -1;
}

void Texture_AllRelease()
{
	for (texture& t : g_Textures)
	{
		SAFE_RELEASE(t.pTexture);
	}
	DBG("Texture_AllRelease: released all textures");
}

void Texture_SetTexture(int texture_id)
{
	//DBG("Texture_SetTexture: id=%d", texture_id);
	if (texture_id <0)
	{
		// デフォルトテクスチャ設定
		//DBG("Texture_SetTexture: setting default texture id=%d", g_DefaultTexture);
		Texture_SetTexture(g_DefaultTexture);
		return;
	}

	if (texture_id >= TEXTURE_MAX) {
		//DBG("Texture_SetTexture: invalid id >= TEXTURE_MAX (%d)", texture_id);
		return;
	}

	// テクスチャ設定
	ID3D11ShaderResourceView* srv = g_Textures[texture_id].pTexture;
	if (!srv) {
		//DBG("Texture_SetTexture: srv is null for id=%d", texture_id);
		return;
	}
	g_pContext->PSSetShaderResources(0,1, &srv);
	//DBG("Texture_SetTexture: bound srv id=%d", texture_id);
}

DirectX::XMUINT2 Texture_GetSize(int texture_id)
{
	return { g_Textures[texture_id].width, g_Textures[texture_id].height };
}

const unsigned int& Texture_GetWidth(int texture_id)
{
	return g_Textures[texture_id].width;
}

const unsigned int& Texture_GetHeight(int texture_id)
{
	return g_Textures[texture_id].height;
}

// Texture class methods (if present in project)
void Texture::CreateBuffers(GraphicsDevice& device)
{
	//DBG("Texture::CreateBuffers: path=%S", m_filePath.c_str());
	TexMetadata metadata{};
	ScratchImage image{};

	HRESULT hr = LoadFromWICFile(m_filePath.c_str(), WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr)) 
	{
		//DBG("Texture::CreateBuffers: LoadFromWICFile FAILED hr=0x%08X", hr);
		throw std::runtime_error("テクスチャの読み込みに失敗しました");
		return;
	}

	m_width = (unsigned int)metadata.width;
	m_height = (unsigned int)metadata.height;

	ScratchImage mipChain{};
	GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);

	image = std::move(mipChain);
	metadata = image.GetMetadata();

	hr = CreateShaderResourceView(device.GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_shaderResourceView);

	if (FAILED(hr))
	{
		//DBG("Texture::CreateBuffers: CreateShaderResourceView FAILED hr=0x%08X", hr);
		throw std::runtime_error("テクスチャの読み込みに失敗しました");
		return;
	}
	//DBG("Texture::CreateBuffers: success size=(%u,%u)", m_width, m_height);
}

void Texture::BindResource(GraphicsDevice& device)
{
	ID3D11ShaderResourceView* srv = m_shaderResourceView.Get();
	device.GetDeviceContext()->PSSetShaderResources(0, 1, &srv);
	//DBG("Texture::BindResource: bound");
}
