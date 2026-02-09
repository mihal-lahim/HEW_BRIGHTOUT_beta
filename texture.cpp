#include "Texture.h"
#include <string>
#include "GraphicsDevice.h"
#include "DirectXTex.h"
#include <Windows.h>

using namespace DirectX;


bool Texture::CreateBuffer(GraphicsDevice& device, const std::wstring& filePath)
{
	m_filePath = filePath;

	// テクスチャデータの読み込み
	TexMetadata metadata{};
	ScratchImage image{};

	// WICを使用して画像ファイルを読み込む
	HRESULT hr = LoadFromWICFile(m_filePath.c_str(), WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr)) 
	{
		return false;
	}

	// テクスチャの幅と高さを保存
	m_width = (UINT)metadata.width;
	m_height = (UINT)metadata.height;

	// ミップマップの生成
	ScratchImage mipChain{};
	GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);

	// 元のイメージをミップマップチェーンに置き換える
	image = std::move(mipChain);
	metadata = image.GetMetadata();

	// シェーダーリソースビューの作成
	hr = CreateShaderResourceView(device.GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_shaderResourceView);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Texture::CreateFromLoaded(GraphicsDevice& device, ID3D11ShaderResourceView* srv, UINT width, UINT height)
{
	(void)device;

	m_shaderResourceView.Attach(srv);
	m_width = width;
	m_height = height;
}

void Texture::Bind(GraphicsDevice& device)
{
	// ピクセルシェーダーにシェーダーリソースビューを設定
	ID3D11ShaderResourceView* srv = m_shaderResourceView.Get();
	device.GetDeviceContext()->PSSetShaderResources(0, 1, &srv);
	ID3D11SamplerState* sampler = device.GetSamplerState();
	device.GetDeviceContext()->PSSetSamplers(0, 1, &sampler);
}
