/*==============================================================================

   動的テクスチャ（動画フレーム表示用）[VideoTexture.cpp]
--------------------------------------------------------------------------------

==============================================================================*/
#include "VideoTexture.h"
#include "GraphicsDevice.h"
#include <Windows.h>
#include <cstring>

bool VideoTexture::CreateBuffer(GraphicsDevice& device, uint32_t width, uint32_t height)
{
	m_width = width;
	m_height = height;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // RGB32(BGRA) に対応
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = device.GetDevice()->CreateTexture2D(&desc, nullptr, &m_texture);
	if (FAILED(hr))
	{
		OutputDebugStringA("[VideoTexture] CreateTexture2D failed\n");
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = device.GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvDesc, &m_srv);
	if (FAILED(hr))
	{
		OutputDebugStringA("[VideoTexture] CreateShaderResourceView failed\n");
		return false;
	}

	OutputDebugStringA("[VideoTexture] CreateBuffer succeeded\n");
	return true;
}

bool VideoTexture::UpdateFromFrame(GraphicsDevice& device,
	const uint8_t* frameData, uint32_t srcStride)
{
	if (!m_texture || !frameData)
		return false;

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = device.GetDeviceContext()->Map(
		m_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
	{
		OutputDebugStringA("[VideoTexture] Map failed\n");
		return false;
	}

	// RGB32（BGRA）形式のフレームデータをコピー
	// MFのRGB32はアルファ未定義（X8R8G8B8相当）の場合があるため、Aを255で上書きする
	const uint8_t* srcRow = frameData;
	uint8_t* dstRow = static_cast<uint8_t*>(mapped.pData);

	for (uint32_t y = 0; y < m_height; ++y)
	{
		const uint8_t* srcPixel = srcRow;
		uint8_t* dstPixel = dstRow;

		for (uint32_t x = 0; x < m_width; ++x)
		{
			// BGRA の各チャンネルをコピー
			dstPixel[0] = srcPixel[0]; // B
			dstPixel[1] = srcPixel[1]; // G
			dstPixel[2] = srcPixel[2]; // R
			dstPixel[3] = 255;         // A（強制不透明）

			srcPixel += 4;
			dstPixel += 4;
		}

		srcRow += srcStride;
		dstRow += mapped.RowPitch;
	}

	device.GetDeviceContext()->Unmap(m_texture.Get(), 0);
	return true;
}

void VideoTexture::Bind(GraphicsDevice& device) const
{
	if (m_srv)
	{
		ID3D11ShaderResourceView* srv = m_srv.Get();
		device.GetDeviceContext()->PSSetShaderResources(0, 1, &srv);

		ID3D11SamplerState* sampler = device.GetSamplerState();
		device.GetDeviceContext()->PSSetSamplers(0, 1, &sampler);
	}
}
