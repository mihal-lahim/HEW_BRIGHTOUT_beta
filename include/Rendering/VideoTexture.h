/*==============================================================================

   動的テクスチャ（動画フレーム表示用）[VideoTexture.h]
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef VIDEO_TEXTURE_H
#define VIDEO_TEXTURE_H

#include <d3d11.h>
#include <cstdint>
#include <wrl/client.h>

class GraphicsDevice;

class VideoTexture
{
public:
	VideoTexture() = default;
	~VideoTexture() = default;

	// 動的テクスチャを作成する
	bool CreateBuffer(GraphicsDevice& device, uint32_t width, uint32_t height);

	// フレームデータでテクスチャを更新する（BGRA/RGB32形式）
	bool UpdateFromFrame(GraphicsDevice& device,
		const uint8_t* frameData, uint32_t srcStride);

	// シェーダーリソースビューをパイプラインにバインドする
	void Bind(GraphicsDevice& device) const;

	// ShaderResourceView を取得する
	ID3D11ShaderResourceView* GetSRV() const { return m_srv.Get(); }

	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;

	uint32_t m_width = 0;
	uint32_t m_height = 0;
};

#endif
